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
#include <pthread.h>
#include "spy.h"
#include "sched.h"
#include "debug.h"

struct spy_init
{
    int cpu;
    int eid;
    spy_thr_t fct;
} init;

pthread_t pth_spy;
volatile int spy_created = 0;

void *spy_init(void *arg)
{
    struct spy_init *init = (struct spy_init*) arg;
    claim_cpu(init->cpu);
    spy_created = 1;

    libsgxstep_info("continuing on CPU %d; calling spy at %p", get_designated_cpu(), init->fct);
    init->fct(init->eid);
}

int spawn_spy(int cpu, spy_thr_t spy, int eid)
{
    init.cpu = cpu;
    init.eid = eid;
    init.fct = spy;
    ASSERT(!pthread_create(&pth_spy, NULL, spy_init, &init));

    while(!spy_created);
    return 0;
}

void join_spy(void)
{
    void *ret;
    ASSERT(pth_spy);
    pthread_join(pth_spy, &ret);
}
