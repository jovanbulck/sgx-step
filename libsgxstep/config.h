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

#define PSTATE_PCT                  100
#define SINGLE_STEP_ENABLE          1
#define APIC_CONFIG_MSR             1

/*
 * XXX Configure APIC timer interval for next interrupt.
 *
 * NOTE: the exact timer interval value depends on CPU frequency, and hence
 *       remains inherently platform-specific. We empirically established
 *       suitable timer intervals on our evaluation platforms by
 *       tweaking and observing the NOP microbenchmark erip results.
 */
#define DELL_INSPIRON_7359          1
#define DELL_OPTIPLEX_7040          2
#define DELL_LATITUDE_7490          3
#if (SGX_STEP_PLATFORM == DELL_INSPIRON_7359)
    #define SGX_STEP_TIMER_INTERVAL 25
    #define VICTIM_CPU              1
#elif (SGX_STEP_PLATFORM == DELL_LATITUDE_7490)
    #define SGX_STEP_TIMER_INTERVAL 36
    #define VICTIM_CPU              6
#elif (SGX_STEP_PLATFORM == DELL_OPTIPLEX_7040)
    #define SGX_STEP_TIMER_INTERVAL 19
    #define VICTIM_CPU              1
#else
    #warning Unsupported SGX_STEP_PLATFORM; configure timer interval manually...
#endif

#endif
