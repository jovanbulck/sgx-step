#ifndef CONFIG_H_INC
#define CONFIG_H_INC

#define TSC_DEADLINE                    1

#define TEST_ITERATIONS                 100001
#define APIC_ONESHOT_TICKS              30*2	// NOTE: sgx-step uses APIC timer divide 2(!)
#define APIC_TSC_INTERVAL               5750

#define USE_IRQ_GATE                    1
#define WRMSR_ON_CPU_LATENCY            0 	// is about 8000

#ifndef IA32_TSC_DEADLINE_MSR
    #define IA32_TSC_DEADLINE_MSR       0x6e0
#endif

#endif
