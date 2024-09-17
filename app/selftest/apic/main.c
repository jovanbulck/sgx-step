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

#include <signal.h>
#include <unistd.h>
#include <x86intrin.h>
#include "libsgxstep/apic.h"
#include "libsgxstep/cpu.h"
#include "libsgxstep/pt.h"
#include "libsgxstep/sched.h"
#include "libsgxstep/enclave.h"
#include "libsgxstep/debug.h"
#include "libsgxstep/config.h"
#include "libsgxstep/idt.h"
#include "libsgxstep/config.h"
#include "config.h"

unsigned long long test_tsc_results[TEST_ITERATIONS];
unsigned long long test_inc_results[TEST_ITERATIONS];
unsigned long long test_deadline_results[TEST_ITERATIONS];

extern uint64_t __ss_irq_rax;
void incs(void);

/* ================== ATTACKER INIT/SETUP ================= */

/* Configure and check attacker untrusted runtime environment. */
void attacker_config_runtime(void)
{
    ASSERT( !claim_cpu(VICTIM_CPU) );
    ASSERT( !prepare_system_for_benchmark(PSTATE_PCT) );
    print_system_settings();

    if (isatty(fileno(stdout)))
    {
        info("WARNING: interactive terminal detected; known to cause ");
        info("unstable timer intervals! Use stdout file redirection for ");
        info("precise single-stepping results...");
    }
}

/* ================== ATTACKER MAIN ================= */

/* Untrusted main function to create/enter the trusted enclave. */
int main( int argc, char **argv )
{
    idt_t idt = {0};
    attacker_config_runtime();
    map_idt(&idt);
    install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);

    #if TSC_DEADLINE
        const char *filename = "deadline_results.txt";
        apic_timer_deadline(IRQ_VECTOR);
    #else
        const char *filename = "oneshot_results.txt";
        apic_timer_oneshot(IRQ_VECTOR);
    #endif

    for (int i = 0; i < TEST_ITERATIONS; ++i)
    {
        __ss_irq_fired = 0;
        g_apic_deadline_tsc_begin = rdtsc_begin();

        #if TSC_DEADLINE && USE_IRQ_GATE
            apic_timer_deadline_irq(APIC_TSC_INTERVAL);
        #elif TSC_DEADLINE
            wrmsr_on_cpu(IA32_TSC_DEADLINE_MSR, get_cpu(),
                         g_apic_deadline_tsc_begin + APIC_TSC_INTERVAL + WRMSR_ON_CPU_LATENCY);
            //unsigned long long wrmsr_time = __rdtsc();
        #else
            apic_timer_irq(APIC_ONESHOT_TICKS);
        #endif

        incs();
        ASSERT(__ss_irq_fired == 1);

	test_tsc_results[i] = nemesis_tsc_aex - g_apic_deadline_tsc_begin ;
	test_inc_results[i] = __ss_irq_rax;
        test_deadline_results[i] = nemesis_tsc_aex - (g_apic_deadline_tsc_begin + APIC_TSC_INTERVAL);
    }

    // record results
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "#tsc_diff,inc_count,tsc_deadline_drift\n");
    for (int i = 1; i < TEST_ITERATIONS; ++i) {
        fprintf(fp, "%llu,%llu,%lld\n", test_tsc_results[i], test_inc_results[i], test_deadline_results[i]);
    }
    fclose(fp);

    return 0;
}
