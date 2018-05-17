#ifndef SGX_STEP_CONFIG
#define SGX_STEP_CONFIG

#define VICTIM_CPU                  1
#define PSTATE_PCT                  100
#define SINGLE_STEP_ENABLE          1

/*
 * XXX Configure APIC timer interval for next interrupt.
 *
 * NOTE: the exact timer interval value depends on CPU frequency, and hence
 *       remains inherently platform-specific. We empirically established
 *       suitable timer intervals on both our evaluation platforms by
 *       tweaking and observing the NOP microbenchmark erip results.
 */
#define DELL_INSPIRON_7359          1
#define DELL_OPTIPLEX_7040          2
#define DELL_LATITUDE_7490          3
#if (SGX_STEP_PLATFORM == DELL_INSPIRON_7359)
    #define SGX_STEP_TIMER_INTERVAL 25
#elif (SGX_STEP_PLATFORM == DELL_LATITUDE_7490)
    #define SGX_STEP_TIMER_INTERVAL 36
#elif (SGX_STEP_PLATFORM == DELL_OPTIPLEX_7040)
    #define SGX_STEP_TIMER_INTERVAL 19
#else
    #warning Unsupported SGX_STEP_PLATFORM; configure timer interval manually...
#endif

#endif
