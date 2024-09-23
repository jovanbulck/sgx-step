#ifndef SGX_STEP_COUNTER_H
#define SGX_STEP_COUNTER_H

struct _counter {
    int irq_cnt;
    int do_irq;
    int fault_cnt;
    int trigger_cnt;
    int step_cnt;
    int aep_cnt;
};

extern struct _counter counter;

#endif
