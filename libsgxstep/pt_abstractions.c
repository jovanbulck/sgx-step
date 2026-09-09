#include "pt_abstractions.h"
#include "pt.h"
#include "debug.h"
#include <sys/mman.h>
#include "enclave.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#define MAX_PAGES 6000
#define MAX_PROTECTED_RANGES 16

typedef struct
{
    size_t start;
    size_t end;

} protected_range_t;

protected_range_t protected_ranges[MAX_PROTECTED_RANGES];
size_t protected_range_count = 0;

static uint64_t *pte_cache[MAX_PAGES] = {NULL};
static uint8_t shifted_pages[MAX_PAGES] = {0};

/*
This function is for internal state keeping

It returns whether a PTE has had their PFN shifted
True: shifted and thus invalid
False: not shifted and thus valid
*/
static inline bool is_page_shifted(size_t pagenum)
{
    return shifted_pages[pagenum] != 0;
}

/*
This function is for internal state keeping

It marks if a PTE has had their PFN shifted,
and thus is invalid
*/
static inline void mark_page_shifted(size_t pagenum)
{
    shifted_pages[pagenum] = 1;
}

/*
This function is for internal state keeping

It unmarks if a PTE has had their PFN unshifted,
and thus is valid
*/
static inline void mark_page_unshifted(size_t pagenum)
{
    shifted_pages[pagenum] = 0;
}

size_t addr_to_pagenum(void *addr)
{
    return (((uintptr_t)addr) - (uintptr_t)get_enclave_base()) / 4096;
}

// This function parses /proc/self/maps to find segments with ---p permissions and adds those to
// the internal state to avoid dereferencing them later which would cause a page fault.
void init_protected_ranges()
{
    FILE *maps = fopen("/proc/self/maps", "r");
    if (!maps)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    uint64_t enclave_base = (uint64_t)get_enclave_base();
    uint64_t enclave_limit = (uint64_t)get_enclave_limit();

    uint64_t start, end;
    char perms[5];
    char *line = NULL;
    size_t len = 0;

    // printf("Registering enclave segments with ---p permissions as protected ranges:\n");

    while (getline(&line, &len, maps) != -1)
    {
        if (sscanf(line, "%lx-%lx %4s", &start, &end, perms) != 3)
            continue;

        if (strcmp(perms, "---p") == 0 &&
            start >= enclave_base && end <= enclave_limit)
        {
            if (protected_range_count >= MAX_PROTECTED_RANGES)
            {
                fprintf(stderr, "Exceeded max protected ranges.\n");
                break;
            }

            size_t start_page = addr_to_pagenum((void *)start);
            size_t end_page = addr_to_pagenum((void *)end);

            protected_ranges[protected_range_count].start = start_page;
            protected_ranges[protected_range_count].end = end_page;
            protected_range_count++;

            // printf("  { %zu, %zu }, // %lx-%lx %s\n", start_page, end_page, start, end, perms);
        }
    }

    free(line);
    fclose(maps);
}

/*
This function is for internal state keeping

It returns whether a certain segment is protected, meaning
that it has no RWX permissions at all

For example: guard pages
*/
int is_protected_segment(size_t page)
{
    static int initialized = 0;
    if (!initialized)
    {
        init_protected_ranges();
        initialized = 1;
    }

    for (size_t i = 0; i < protected_range_count; i++)
    {
        if (page >= protected_ranges[i].start && page <= protected_ranges[i].end)
            return 1;
    }
    return 0;
}

// If the user bit is cleared and we write to a data page, no page fault in userspace is triggered
// which results in the attack halting and not making any progress
// Best guess: Happens due to kernel internals
// This is why we are revoking access by changing the PFN

// We revoke access by invalidating the physical address of the page, which sgx will throw a pagefault error on.
int pte_removeperms_PFN(size_t pagenumber)
{
    if (is_page_shifted(pagenumber))
    {
        // If the access has already been revoked, do not shift further.
        // Otherwise the internal state of this module is invalid.
        return -1;
    }
    uint64_t *pte = get_pte_by_page(pagenumber);

    if (!pte)
        return -1; // Error retrieving PTE

    uint64_t phys_addr = *pte & PT_PHYS_MASK;

    // Add the offset (0x1000) to make invalid
    uint64_t invalid_phys_addr = phys_addr + 0x1000;

    // Preserve flags and update PTE
    *pte = (*pte & ~PT_PHYS_MASK) | (invalid_phys_addr & PT_PHYS_MASK);

    mark_page_shifted(pagenumber);
}

int pte_restoreperms_PFN(size_t pagenumber)
{
    if (!is_page_shifted(pagenumber))
    {
        return -1;
    }
    uint64_t *pte = get_pte_by_page(pagenumber);

    if (!pte)
        return -1; // Error retrieving PTE

    uint64_t old_phys_addr = *pte & PT_PHYS_MASK;

    // Subtract the offset to make valid again
    uint64_t new_phys_addr = old_phys_addr - 0x1000;

    // Preserve flags and update PTE
    *pte = (*pte & ~PT_PHYS_MASK) | (new_phys_addr & PT_PHYS_MASK);
    mark_page_unshifted(pagenumber);
}

// Not used currently
int pte_removeperms_SUPERVISOR(uint64_t *pte_pointer)
{
    if (!PRESENT(*pte_pointer))
    {
        return -1;
    }

    *pte_pointer = MARK_SUPERVISOR(*pte_pointer);
    return 1;
}

// Not used currently
int pte_restoreperms_SUPERVISOR(uint64_t *pte_pointer)
{
    if (!PRESENT(*pte_pointer))
    {
        return -1;
    }
    *pte_pointer = MARK_USER(*pte_pointer);
    return 1;
}

// Converts a page number into the corresponding virtual address
static inline void *pagenum_to_virt(size_t page)
{
    // Both implementations are equivalent:
    return (void *)((page << PT_SHIFT) + (size_t)get_enclave_base());
    // return (size_t)get_enclave_base() + page * PAGE_SIZE_4KiB;
}

uint64_t *get_pte_by_page(size_t page)
{
    if (page >= MAX_PAGES)
    {
        debug("Out of bounds of the page cache, increase MAX_PAGES in pt_abstractions.c");
        return NULL; // Out of bounds
    }

    // If the page is in a protected segment, so no RWX permissions do not keep track of it
    // Otherwise later in this function on the dereference step an error will occur.
    if (is_protected_segment(page))
    {
        return NULL;
    }

    // If entry not in the cache, add it
    if (!pte_cache[page])
    {
        void *virt_addr = pagenum_to_virt(page);

        if (!(get_enclave_base() < virt_addr && virt_addr < get_enclave_limit()))
        {
            debug("Page %lu out of range, not part of the enclave", page);
            return NULL;
        }

        // Forcing the page to be present
        volatile uint64_t force_page_to_be_present = *(uint64_t *)virt_addr;

        // mlock the page so it does not get unmapped
        mlock(virt_addr, 4096);

        // Remapping into user space
        uint64_t *pte = remap_page_table_level(virt_addr, PTE);

        // Adding it to the cache
        pte_cache[page] = pte;
    }

    // Return the entry from the cache
    return pte_cache[page];
}

uint64_t *get_pte_by_address(void *virt_addr)
{
    size_t page_num = virt_to_pagenum(virt_addr);
    return get_pte_by_page(page_num);
}

int pte_revoke_pages(size_t page, size_t num_pages)
{
    for (size_t i = 0; i < num_pages; i++)
    {
        pte_removeperms_PFN(page + i);
    }
    return 0; // Success
}

int pte_restore_pages(size_t page, size_t num_pages)
{
    for (size_t i = 0; i < num_pages; i++)
    {
        pte_restoreperms_PFN(page + i);
    }
    return 0; // Success
}