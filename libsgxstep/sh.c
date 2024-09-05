#include "sh.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"

// One signal_handler_cb per signo
signal_handler_t signal_handler_cb[32] = {NULL};

void signal_handler_wrapper(int signo, siginfo_t *info, void *context) {
    ASSERT(!(info == NULL || info->si_addr == NULL));

    ucontext_t *u_context = (ucontext_t *)context;

    if (signal_handler_cb[signo]) {
#if DEBUG
        info("Calling handler for signal %d", signo);
#endif
        signal_handler_cb[signo](info, u_context);
    } else {
        info("Caught unregistered signal %d", signo);
        abort();
    }
}

void register_signal_handler(signal_handler_t cb, int signo) {
    struct sigaction act, old_act;

    /* Specify handler with signinfo arguments */
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_sigaction = signal_handler_wrapper;
    act.sa_flags = SA_RESTART | SA_SIGINFO;

    /* Block all signals while the signal is being handled */
    sigfillset(&act.sa_mask);
    ASSERT(!sigaction(signo, &act, &old_act));

    signal_handler_cb[signo] = cb;
#if DEBUG
    info("Registered a handler for signal %d", signo);
#endif
}
