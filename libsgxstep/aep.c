#include "debug.h"
#include "aep.h"
#define __USE_GNU
#include <signal.h>
#include <string.h>

/* See aep_trampoline.S to see how these are used. */
extern void xs_enclu_stub(void);
extern void sgx_step_aep_eresume(void);
uint64_t *xs_enclu_cont_inst = NULL;

void* xs_aep_pt = sgx_step_aep_eresume;
void* xs_tcs_pt = NULL;

void* xs_get_aep(void)
{
    return xs_aep_pt;
}

void xs_set_aep(void* aep)
{
    xs_aep_pt = aep;
}

void* xs_get_tcs(void)
{
    return xs_tcs_pt;
}

/*
 * TODO 
 *      1/ integrate this with unified signal handler <https://github.com/jovanbulck/sgx-step/pull/85>
 *      2/ make sure this also works when the application defines its own (trap) signal handlers
 */
void xs_sigtrap_handler(int signo, siginfo_t * si, void  *ctx)
{
    /* /usr/include/x86_64-linux-gnu/sys/ucontext.h */
    ucontext_t *uctx = (ucontext_t *) ctx;
    void *rip = (void*) uctx->uc_mcontext.gregs[REG_RIP];
    void *rax = (void*) uctx->uc_mcontext.gregs[REG_RAX];
    void *rbx = (void*) uctx->uc_mcontext.gregs[REG_RBX];
    void *rcx = (void*) uctx->uc_mcontext.gregs[REG_RCX];

    info("signal handler rip %p; rax=%p; rbx=%p; rcx=%p", rip, rax, rbx, rcx);
    switch ( signo )
    {
      case SIGTRAP:
        uint8_t trap_inst = *(((uint8_t*) ((uint8_t*)rip)-1));
        uint8_t next_inst = *((uint8_t*) rip);
        xs_enclu_cont_inst = rip;
        char *reason;
        switch ((uint64_t) rax)
        {
            case 2:
                reason = "EENTER";
                break;
            case 3:
                reason = "ERESUME";
                break;
            default:
                reason = "OTHER";
        }
        xs_tcs_pt = (uint64_t*) rbx;

        info("Caught trap at %p (disasm: %#x %#x) tcs=%p; aep=%p; reason=%s",
            rip, trap_inst, next_inst, rbx, rcx, reason);
        info("\t.. redirecting instruction pointer to enclu_stub at %p", &xs_enclu_stub);
        uctx->uc_mcontext.gregs[REG_RIP] = (uint64_t) &xs_enclu_stub;
        return;

      default:
        info("Caught unexpected signal '%d'", signo);
        abort();
    }
}

void __attribute__((constructor)) xs_register_fault_handler(void)
{
    struct sigaction act, old_act;
    memset(&act, 0, sizeof(sigaction));

    act.sa_sigaction = xs_sigtrap_handler;
    act.sa_flags = SA_RESTART | SA_SIGINFO;

    /* Block all signals while the signal is being handled */
    sigfillset(&act.sa_mask);
    info("installing sigtrap handler to intercept ENCLU..");
    ASSERT(!sigaction(SIGTRAP, &act, &old_act));
}
