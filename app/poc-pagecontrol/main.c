/* utility headers */
#include "libsgxstep/debug.h"
#include "libsgxstep/pf.h"
#include <sys/mman.h>
#include "libsgxstep/pt.h"
#include "libsgxstep/enclave.h"
#include "libsgxstep/pt_abstractions.h"
#include "libsgxstep/sched.h"
#include "libsgxstep/pf_abstractions.h"

/* Example Setup */
#include "Enclave/config_example.h"

/* SGX untrusted runtime */
#include <sgx_urts.h>
#include "Enclave/encl_u.h"

sgx_enclave_id_t create_enclave(void)
{
    sgx_launch_token_t token = {0};
    int updated = 0;
    sgx_enclave_id_t eid = -1;

#if DEBUG
    info_event("Creating enclave...");
#endif
    SGX_ASSERT(sgx_create_enclave("./Enclave/encl.so", /*debug=*/1,
                                  &token, &updated, &eid, NULL));

    return eid;
}

void fault_handler(size_t pagenum)
{
#if DEBUG
    info("Page fault on page %d:", pagenum);
#endif

// Restore page
#if USE_MPROTECT
    restore_pages(pagenum, 1);
#else
    pte_restore_pages(pagenum, 1);
#endif
}

int main(int argc, char **argv)
{
    // -----------------------------------------------------------
    // SETUP
    // -----------------------------------------------------------
    sgx_enclave_id_t eid = create_enclave();
#if DEBUG
    info("Setup");
#endif
#if USE_CUSTOM_IDT
    register_fault_handler_IDT(fault_handler);
#else
    register_fault_handler(fault_handler);
#endif
    void *buff_addrs;
    SGX_ASSERT(ecall_leak_internal_buffer_adrs(eid, &buff_addrs));
#if DEBUG
    info("Buffer addrs: %p", buff_addrs);
#endif

#if DEBUG
    info_event("Revoking access to target page.");
#endif
#if USE_MPROTECT
    revoke_pages(virt_to_pagenum(buff_addrs), 1);
#else
    pte_revoke_pages(virt_to_pagenum(buff_addrs), 1);
#endif
#if DEBUG
    info_event("Calling enclave to access target page.");
#endif

    SGX_ASSERT(ecall_access_target_page(eid);

#if DEBUG
    info_event("destroying SGX enclave");
#endif
    SGX_ASSERT(sgx_destroy_enclave(eid));

    return 0;
}
