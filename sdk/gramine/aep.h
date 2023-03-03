#ifndef SGX_STEP_AEP_H
#define SGX_STEP_AEP_H


#ifndef NUM_RUNS
    #define NUM_RUNS                500
#endif

void fault_handler(int signal);

void attacker_config_runtime(void);

void aep_cb_func(void);

void attacker_config_page_table(void);

void restore_timer(void);

void configure_mapping(void);

#endif
