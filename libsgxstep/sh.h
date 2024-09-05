#ifndef SGX_STEP_SH_H
#define SGX_STEP_SH_H

#include <signal.h>
#include <stddef.h>

// Define a type for the fault handler callback function
typedef void (*signal_handler_t)(siginfo_t *, ucontext_t *);

// Declare the external fault handler callback variable
extern signal_handler_t signal_handler_cb[32];

// Function to register a fault handler
void register_signal_handler(signal_handler_t handler, int signo);

#endif  // PF_H
