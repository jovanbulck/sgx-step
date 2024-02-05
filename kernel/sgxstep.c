/*
 *  This file is part of the SGX-Step enclave execution control framework.
 *
 *  Copyright (C) 2017 Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>,
 *                     Raoul Strackx <raoul.strackx@cs.kuleuven.be>
 *
 *  SGX-Step is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SGX-Step is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SGX-Step. If not, see <http://www.gnu.org/licenses/>.
 */

#include "sgxstep_internal.h"
#include "sgxstep_ioctl.h"

#include <asm/pgtable.h>
#include <asm/page.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <asm/irq.h>
#include <asm/apic.h>

#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/slab.h>

#include <linux/clockchips.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>, Raoul Strackx <raoul.strackx@cs.kuleuven.be>");
MODULE_DESCRIPTION("SGX-Step: A Practical Attack Framework for Precise Enclave Execution Control");

static struct page **g_isr_pages = NULL;
static uint64_t g_isr_nr_pages = 0;
static void *g_isr_kernel_vbase = NULL;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5,6,0))
    #define pin_user_pages_fast         get_user_pages_fast
    #define unpin_user_pages            put_user_pages
#endif

static int g_in_use = 0;

typedef struct {
    uint16_t size;
    uint64_t base;
} __attribute__((packed)) idtr_t;
static idtr_t g_idtr = {0};
static void *g_idt_copy = NULL;

static uint32_t g_apic_lvtt_copy = 0x0, g_apic_tdcr_copy = 0x0;

/* ********************** UTIL FUNCTIONS ******************************* */

/*
 * NOTE: Linux's default `write_cr0` does not allow to change protected bits,
 * so we include our own here.
 */
inline void do_write_cr0(unsigned long val) {
    asm volatile("mov %0, %%cr0" : : "r"(val));
}

void enable_write_protection(void)
{
    unsigned long cr0 = read_cr0();
    set_bit(16, &cr0);
    do_write_cr0(cr0);
}

void disable_write_protection(void)
{
    unsigned long cr0 = read_cr0();
    clear_bit(16, &cr0);
    do_write_cr0(cr0);
}

/* ********************** DEVICE OPEN ******************************* */

/*
 * Save original interrupt descriptor table register (IDTR) -- remains
 * unmodified by libsgxstep; addresses the single global IDT memory structure
 * setup by Linux at boot time and shared accross all CPUs.
 *
 * Copy original interrupt descriptor table (IDT) -- may be modified by
 * libsgxstep; will be auto restored when closing /dev/sgx-step.
 */
int save_idt(void)
{
    asm volatile ("sidt %0\n\t"
                  :"=m"(g_idtr) :: );
    log("original IDT: %#llx with size %u", g_idtr.base, g_idtr.size+1);
    RET_ASSERT(g_idtr.base);

    g_idt_copy = kmalloc(g_idtr.size+1, GFP_KERNEL);
    RET_ASSERT(g_idt_copy);
    memcpy(g_idt_copy, (void*)g_idtr.base, g_idtr.size+1);

    return 0;
}

/*
 * Save APIC timer configuration registers that may be modified by libsgxstep;
 * will be auto restored when closing /dev/sgx-step.
 */
int save_apic(void)
{
    g_apic_lvtt_copy = apic->read(APIC_LVTT);
    g_apic_tdcr_copy = apic->read(APIC_TDCR);
    log("original APIC_LVTT=%#x/TDCR=%#x)", g_apic_lvtt_copy, g_apic_tdcr_copy);

    return 0;
}

int step_open(struct inode *inode, struct file *file)
{
    if (g_in_use)
    {
        err("Device is already opened");
        return -EBUSY;
    }

    RET_ASSERT( !save_idt() );
    RET_ASSERT( !save_apic() );

    g_in_use = 1;
    return 0;
}

/* ********************** DEVICE CLOSE ******************************* */

/*
 * Restore original IDT to ensure no user pointers are left. Free kernel vbase
 * mappings and pinned user physical pages for any registered user ISRs.
 *
 * NOTE: the IDT virtual memory page is mapped write-protected by Linux, so we
 * have to disable CR0.WP temporarily here.
 */
void restore_idt(void)
{
    disable_write_protection();
    memcpy((void*)g_idtr.base, g_idt_copy, g_idtr.size+1);
    enable_write_protection();
    log("restored IDT: %#llx with size %u", g_idtr.base, g_idtr.size+1);

    kfree(g_idt_copy);
    g_idt_copy = NULL;

    if (g_isr_kernel_vbase) {
        vunmap(g_isr_kernel_vbase);
        g_isr_kernel_vbase = NULL;
        
        unpin_user_pages(g_isr_pages, g_isr_nr_pages);
        g_isr_nr_pages = 0;
        
        kfree(g_isr_pages);
        g_isr_pages = NULL;
    }
}

void restore_apic(void)
{
    int delta = 100;

    apic->write(APIC_LVTT, g_apic_lvtt_copy);
    apic->write(APIC_TDCR, g_apic_tdcr_copy);
    log("restored APIC_LVTT=%#x/TDCR=%#x)", g_apic_lvtt_copy, g_apic_tdcr_copy);

    /* In xAPIC mode the memory-mapped write to LVTT needs to be serialized. */
    asm volatile("mfence" : : : "memory");

    /* Re-arm the timer so Linux's original handler should take over again. */
    if (g_apic_lvtt_copy & APIC_LVT_TIMER_TSCDEADLINE)
    {
        log("restoring APIC timer tsc-deadline operation");
        wrmsrl(MSR_IA32_TSC_DEADLINE, rdtsc() + delta);
    }
    else
    {
        log("restoring APIC timer one-shot/periodic operation");
        apic->write(APIC_TMICT, delta);
    }
}

/*
 * Called when /dev/sgx-step is closed, also when the application that
 * originally opened it crashed. We take care to restore any IDT and APIC
 * modifications made by user-space libsgxstep here to their original values,
 * such that everything runs again normally and Linux does not panic.
 */
int step_release(struct inode *inode, struct file *file)
{
    restore_idt();
    restore_apic();

    g_in_use = 0;
    return 0;
}

/* ********************** IOCTL FUNCTIONS ******************************* */

/* Convenience function when editing PTEs from user space (but normally not
 * needed, since SGX already flushes the TLB on enclave entry/exit) */
long sgx_step_ioctl_invpg(struct file *filep, unsigned int cmd, unsigned long arg)
{
    uint64_t addr = ((invpg_t *) arg)->adrs;

    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");

    return 0;
}

long sgx_step_get_pt_mapping(struct file *filep, unsigned int cmd, unsigned long arg)
{
    address_mapping_t *map = (address_mapping_t*) arg;
	pgd_t *pgd = NULL;
	pud_t *pud = NULL;
	pmd_t *pmd = NULL;
	pte_t *pte = NULL;
    #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0))
        p4d_t *p4d = NULL;
    #endif

	uint64_t virt;
    RET_ASSERT(map);
	
	virt = map->virt;
	memset( map, 0x00, sizeof( address_mapping_t ) );
	map->virt = virt;
	
	map->pgd_phys_address = __pa( current->mm->pgd );
	pgd = pgd_offset( current->mm, virt );
	map->pgd = *((uint64_t *) pgd);
	
	if ( !pgd_present( *pgd ) )
		return 0;

    #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0))
        #if CONFIG_PGTABLE_LEVELS > 4
            #warning 5-level page tables currently not supported by SGX-Step
            #warning unfolding dummy p4d; try rebooting with `no5lvl` kernel parameter if needed
        #endif

        /* simply unfold the pgd inside the dummy p4d struct */
        p4d = p4d_offset( pgd, virt);
        pud = pud_offset( p4d, virt );
    #else
        pud = pud_offset( pgd, virt );
    #endif

	map->pud = *((uint64_t *) pud);
	
	if ( !pud_present( *pud ) )
		return 0;
	
	pmd = pmd_offset( pud, virt );
	map->pmd = *((uint64_t *) pmd);
	
	if ( !pmd_present( *pmd ) )
		return 0;
	
	pte = pte_offset_kernel( pmd, virt );
	map->pte = *((uint64_t *) pte);
	
	if ( !pte_present( *pte ) )
		return 0;
	
	map->phys = PFN_PHYS( pte_pfn( *pte ) ) | ( virt & 0xfff );

    return 0;
}

long sgx_step_ioctl_setup_isr_map(struct file *filep, unsigned int cmd, unsigned long arg)
{
    uint64_t nr_pinned_pages;
    setup_isr_map_t *data = (setup_isr_map_t*) arg;

    /* allocate space to hold Linux `struct page` pointers */
    g_isr_nr_pages = (data->isr_stop - data->isr_start + PAGE_SIZE - 1) / PAGE_SIZE;
    g_isr_pages = kmalloc(g_isr_nr_pages * sizeof(struct page *), GFP_KERNEL);
    GOTO_ASSERT(g_isr_pages, "cannot allocate memory", out);

    /* pin user physical memory so it cannot be swapped out by the kernel */
    nr_pinned_pages = pin_user_pages_fast(data->isr_start & ~(PAGE_SIZE - 1),
                        g_isr_nr_pages, FOLL_LONGTERM | FOLL_WRITE, g_isr_pages);
    GOTO_ASSERT(nr_pinned_pages == g_isr_nr_pages, "cannot pin all ISR pages", cleanup_pages);

    /* map pinned physical memory into the kernel virtual address range */
    g_isr_kernel_vbase = vmap(g_isr_pages, g_isr_nr_pages,
                            VM_READ | VM_EXEC | VM_SHARED, PAGE_SHARED_EXEC);
    GOTO_ASSERT(g_isr_kernel_vbase, "cannot vmap ISR pages", cleanup_pin);

    data->isr_kernel_base = (uint64_t) g_isr_kernel_vbase;
    log("mapped %lld pinned user ISR memory pages to kernel virtual address %#llx",
            g_isr_nr_pages, data->isr_kernel_base);
    return 0;

cleanup_pin:
    unpin_user_pages(g_isr_pages, g_isr_nr_pages);
    
cleanup_pages:
    kfree(g_isr_pages);

out:
    g_isr_kernel_vbase = NULL;
    g_isr_pages = NULL;
    g_isr_nr_pages = 0;
    return -EINVAL;
}

typedef long (*ioctl_t)(struct file *filep, unsigned int cmd, unsigned long arg);

long step_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    char data[256];
    ioctl_t handler = NULL;
    long ret;

    switch (cmd)
    {
        case SGX_STEP_IOCTL_GET_PT_MAPPING:
            handler = sgx_step_get_pt_mapping;
            break;
        case SGX_STEP_IOCTL_INVPG:
            handler = sgx_step_ioctl_invpg;
            break;
        case SGX_STEP_IOCTL_SETUP_ISR_MAP:
            handler = sgx_step_ioctl_setup_isr_map;
            break;
        default:
            return -EINVAL;
    }

    RET_ASSERT(handler && (_IOC_SIZE(cmd) < 256));
    if (copy_from_user(data, (void __user *) arg, _IOC_SIZE(cmd)))
        return -EFAULT;

    ret = handler(filep, cmd, (unsigned long) ((void *) data));

    if (!ret && (cmd & IOC_OUT)) {
        if (copy_to_user((void __user *) arg, data, _IOC_SIZE(cmd)))
            return -EFAULT;
    }

    return ret;
}

/* ********************** FILE OPERATIONS ******************************* */

static const struct file_operations step_fops = {
    .owner              = THIS_MODULE,
    .compat_ioctl       = step_ioctl,
    .unlocked_ioctl     = step_ioctl,
    .open               = step_open,
    .release            = step_release
};

static struct miscdevice step_dev = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = DEV,
    .fops   = &step_fops,
    .mode   = S_IRUGO | S_IWUGO
};

/* Code from: <https://www.libcrack.so/index.php/2012/09/02/bypassing-devmem_is_allowed-with-kprobes/> */
static int devmem_is_allowed_handler (struct kretprobe_instance *rp, struct pt_regs *regs)
{
    if (regs->ax == 0) {
        regs->ax = 0x1;
    }
    return 0;
}

static struct kretprobe krp = {
    .handler = devmem_is_allowed_handler,
    .maxactive = 20 /* Probe up to 20 instances concurrently. */
};

int init_module(void)
{
    /* Register virtual device */
    if (misc_register(&step_dev))
    {
        err("virtual device registration failed..");
        step_dev.this_device = NULL;
        return -EINVAL;
    }

    /* Activate a kretprobe to bypass CONFIG_STRICT_DEVMEM kernel compilation option */
    krp.kp.symbol_name = "devmem_is_allowed";
    if (register_kretprobe(&krp) < 0)
    {
        err("register_kprobe failed..");
        step_dev.this_device = NULL;
        return -EINVAL;
    }

    log("listening on /dev/" DEV);
    return 0;
}

void cleanup_module(void)
{
    /* Unregister virtual device */
    if (step_dev.this_device)
        misc_deregister(&step_dev);

    unregister_kretprobe(&krp);
    log("kernel module unloaded");
}
