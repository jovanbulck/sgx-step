#ifndef SGX_STEP_PF_ABSTRACTIONS_H
#define SGX_STEP_PF_ABSTRACTIONS_H

#include "pf.h"

void register_fault_handler_IDT(fault_handler_t cb);

void __pf_irq_handler(void);
extern uint64_t volatile __pf_irq_original_handler_addr;
extern uint64_t volatile __pf_irq_faultaddr;

#endif