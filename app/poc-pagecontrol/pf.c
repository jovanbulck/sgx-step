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

#include "libsgxstep/debug.h"
#include "pf.h"
#include "libsgxstep/pt.h"
#include "libsgxstep/enclave.h"
#include <signal.h>
#include <string.h>
#include <sys/mman.h>

fault_handler_t __fault_handler_cb = NULL;

void fault_handler_wrapper(int signo, siginfo_t *si, void *ctx)
{
    void *base_adrs;

    switch (signo)
    {
    case SIGSEGV:
        base_adrs = si->si_addr;
        break;

    default:
        info("Caught unknown signal '%d'", signo);
        return;
    }

    size_t page_num =
        ((size_t)base_adrs - (size_t)get_enclave_base()) >> PT_SHIFT;

    if (__fault_handler_cb)
        __fault_handler_cb(page_num);
}

void register_fault_handler(fault_handler_t cb)
{
    struct sigaction act, old_act;
    memset(&act, 0, sizeof(sigaction));

    act.sa_sigaction = fault_handler_wrapper;
    act.sa_flags = SA_RESTART | SA_SIGINFO;

    // Block all signals while the signal is being handled
    sigfillset(&act.sa_mask);

    ASSERT(!sigaction(SIGSEGV, &act, &old_act));

    __fault_handler_cb = cb;
}

// TODO: Do something more clever by hacking PTEs directly?
int protect_pages(size_t page, size_t num_pages, int prot)
{
    size_t start = ((size_t)get_enclave_base() + page * PAGE_SIZE_4KiB);
    size_t len = num_pages * PAGE_SIZE_4KiB;
    return mprotect((void *)start, len, prot);
}

int revoke_pages(size_t page, size_t num_pages)
{
    return protect_pages(page, num_pages, PROT_NONE);
}

int restore_pages(size_t page, size_t num_pages)
{
    // Returns 0 early if any protect succeeds
    return protect_pages(page, num_pages, PROT_READ | PROT_WRITE | PROT_EXEC) && protect_pages(page, num_pages, PROT_READ | PROT_EXEC) && protect_pages(page, num_pages, PROT_READ | PROT_WRITE) && protect_pages(page, num_pages, PROT_READ);
}
