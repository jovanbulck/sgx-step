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

#define _GNU_SOURCE
#include <sched.h>
#include "sched.h"
#include "debug.h"
#include "sys_file.h"

int claim_cpu(int cpu)
{
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    ASSERT(sched_setaffinity(0, sizeof(cpuset), &cpuset) >= 0);
    return 0;
}

int get_designated_cpu( void )
{
	cpu_set_t set;
	int i;
	
	if ( sched_getaffinity( 0, sizeof( cpu_set_t ), &set ) )
		return -1;
	
	if ( CPU_COUNT( &set ) != 1 )
		return -2;
	
	for ( i = 0; i < CPU_SETSIZE; ++i )
	{
		if ( CPU_ISSET( i, &set ) )
			return i;
	}
	
	return -3;
}

unsigned int pstate_max_perf_pct( void )
{
#if HAS_PSTATE
	int result;
	
	if ( 0 != fread_int( "/sys/devices/system/cpu/intel_pstate/max_perf_pct", &result ) )
		return -1;
	
	return result;
#else
	return 0;
#endif
}

int pstate_set_max_perf_pct( unsigned int val)
{
#if HAS_PSTATE
	return fwrite_value( "/sys/devices/system/cpu/intel_pstate/max_perf_pct", (int) val);
#else
    return -1;
#endif
}

unsigned int pstate_min_perf_pct( void )
{
#if HAS_PSTATE
	int result;
	
	if ( 0 != fread_int( "/sys/devices/system/cpu/intel_pstate/min_perf_pct", &result ) )
		return -1;
	
	return result;
#else
	return 0;
#endif
}

int pstate_set_min_perf_pct( unsigned int val)
{
#if HAS_PSTATE
	return fwrite_value( "/sys/devices/system/cpu/intel_pstate/min_perf_pct", (int) val);
#else
    return -1;
#endif
}

int disable_turbo(void)
{
#ifdef HAS_TURBO
    return fwrite_value( "/sys/devices/system/cpu/intel_pstate/no_turbo", 1);
#else
    return -1;
#endif
}

int turbo_enabled( void )
{
	int result;
	
	if ( 0 != fread_int( "/sys/devices/system/cpu/intel_pstate/no_turbo", &result ) )
		return -1;
	
	return (result == 0)? 1 : 0;
}

int prepare_system_for_benchmark(void)
{
    return !pstate_set_min_perf_pct(100) && !pstate_set_max_perf_pct(100) && !disable_turbo();
}

int print_system_settings(void)
{
    int pin = 0;

	printf( "==== System Settings ====\n" );
	printf( "    Pstate max perf pct: %d\n", pstate_max_perf_pct() );
	printf( "    Pstate min perf pct: %d\n", pstate_min_perf_pct() );
	printf( "    Turbo Boost:         %d\n", turbo_enabled() );

	printf( "    cpu pinning:         %d\n", pin = (get_designated_cpu() >= 0));
	if (pin)
    printf( "    Designated cpu:      %d\n", get_designated_cpu() );
	printf( "    Running on cpu:      %d\n", sched_getcpu()  );
}
