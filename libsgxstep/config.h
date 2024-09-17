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

#ifndef SGX_STEP_CONFIG
#define SGX_STEP_CONFIG

// TODO add coarser-grained logging levels
#define LIBSGXSTEP_DEBUG            0

#define PSTATE_PCT                  100
#define SINGLE_STEP_ENABLE          1
#define USER_IDT_ENABLE             1
#define IRQ_VECTOR                  45
#define IRQ_PRIV_VECTOR             49
#define RDMSR_GATE_VECTOR           50
#define WRMSR_GATE_VECTOR           51
#define GDT_VECTOR                  13

/*
 * Some recent CPUs with LEGACY_XAPIC_DISABLED only support x2apic mode with
 * SGX. In x2apic mode all APIC configuration needs to go through ring-0
 * RD/WRMSR instructions, which is also the only access mode supported for
 * IA32_TSC_DEADLINE.
 */
#define X2APIC                      1
#if (!X2APIC && !M32)
    #define APIC_CONFIG_MSR         1
#else
    #define APIC_CONFIG_MSR         0
#endif

#define VICTIM_CPU                  1
#define NUM_CORES                   4
#define SPY_CPU                     (NUM_CORES-1)

/*
 * XXX Configure APIC timer interval for next interrupt.
 *
 * NOTE: the exact timer interval value depends on CPU frequency, and hence
 *       remains inherently platform-specific. We empirically established
 *       suitable timer intervals on our evaluation platforms by
 *       tweaking and observing the NOP microbenchmark erip results.
 */
#define SGX_STEP_TIMER_INTERVAL 53

#endif
