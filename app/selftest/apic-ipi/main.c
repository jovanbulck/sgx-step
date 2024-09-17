#include <fcntl.h>
#include "libsgxstep/debug.h"
#include "libsgxstep/config.h"
#include "libsgxstep/spy.h"
#include "libsgxstep/sched.h"
#include "libsgxstep/pt.h"
#include "libsgxstep/apic.h"
#include "libsgxstep/idt.h"
#include <string.h>
#include <sgx_urts.h>
#include <x86intrin.h>

#define SAMPLES         100000
#define WARMUP_ROUNDS   1
#define SELF_IPI        0

#define MY_VICTIM_CPU   1
#define MY_SPY_CPU      3       // no hyperthreading
//#define MY_SPY_CPU      37       // hyperthreading

#if SELF_IPI
    #define DUMP_FILE       "ipi-self.csv"
#else
    #define DUMP_FILE       "ipi-cross.csv"
#endif

/* see asm.S */
void apic_write_icr_ret(uint32_t icr_low, uint32_t icr_high);
void apic_write_icr_nop(uint32_t icr_low, uint32_t icr_high);
void apic_write_nop_slide_start(void);
void apic_write_nop_slide_end(void);

unsigned long long test_tsc_results[SAMPLES] = {0};
unsigned long long test_inc_results[SAMPLES] = {0};

/* ******************** SPY LOGIC ******************** */
volatile int spy_ready = 0, victim_ready = 0, victim_stop = 0;
uint8_t apic_id_victim = 0;
int spurious_irq = 0;

void spy_func(int eid)
{
    info_event("triggering IPIs from CPU %d (core %d) to CPU %d (core %d)",
                get_cpu(), get_core_id(get_cpu()), MY_VICTIM_CPU, get_core_id(MY_VICTIM_CPU));

    for (int i = -WARMUP_ROUNDS; i < SAMPLES; i++)
    {
        #if !SELF_IPI
            while(!victim_ready);
            spy_ready = 1;
        #endif

        __ss_irq_fired = 0;
        unsigned long long begin_time = __rdtsc();

        #if SELF_IPI
            apic_write_icr_nop(APIC_ICR_VECTOR(IRQ_VECTOR) |
                               APIC_ICR_DELIVERY_FIXED | APIC_ICR_LEVEL_ASSERT |
                               APIC_ICR_DEST_SELF, 0x0);

            ASSERT( __ss_irq_fired );
        #else
            apic_write_icr_ret(APIC_ICR_VECTOR(IRQ_VECTOR) |
                               APIC_ICR_DELIVERY_FIXED | APIC_ICR_LEVEL_ASSERT |
                               APIC_ICR_DEST_PHYSICAL,
                               #if !X2APIC
                                 (apic_id_victim << APIC_ID_SHIFT) & APIC_ICR_DEST_MASK
                               #else
                                 apic_id_victim
                               #endif
                              );

            spy_ready = 0;
            while( !__ss_irq_fired );
        #endif

        if (__ss_irq_rip < (uint64_t) apic_write_nop_slide_start ||
            __ss_irq_rip > (uint64_t) apic_write_nop_slide_end)
        {
            info("WARNING: spurious IRQ outside NOP slide at %p", __ss_irq_rip);
            spurious_irq++;
            i--;
            continue;
        }

        /* first warmup round always takes longer, so we discard the first measurement here */
        if (i < 0) continue;

        uint64_t offset = __ss_irq_rip - (uint64_t) apic_write_nop_slide_start;
        test_inc_results[i] = offset;
	test_tsc_results[i] = nemesis_tsc_aex - begin_time;
    }
    victim_stop = 1;
}

/* ******************** VICTIM LOGIC ******************** */

#if !SELF_IPI
void victim_func(void)
{
    while (!victim_stop)
    {
        victim_ready = 1;
        while (!spy_ready);
        victim_ready = 0;

        apic_write_nop_slide_start();
    }
}
#endif

/* ================== ATTACKER INIT/SETUP ================= */

/* Configure and check attacker untrusted runtime environment. */
void attacker_config_runtime(void)
{
    ASSERT( !claim_cpu(MY_VICTIM_CPU) );
    ASSERT( !prepare_system_for_benchmark(PSTATE_PCT) );
    print_system_settings();

    if (isatty(fileno(stdout)))
    {
        info("WARNING: interactive terminal detected; known to cause ");
        info("unstable timer intervals! Use stdout file redirection for ");
        info("precise single-stepping results...");
    }
}

/* ================== ATTACKER MAIN ================= */

int main(int argc, char **argv )
{
    idt_t idt = {0};
    FILE *fd;
    attacker_config_runtime();

    info_event("installing IRQ handler");
    map_idt(&idt);
    install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);

    info_event("mapping APIC");
    apic_id_victim = apic_id();
    info("victim CPU=%d with APIC_ID=%#x", get_cpu(), apic_id_victim);

#if !SELF_IPI
    info_event("setting up spy and victim threads");
    ASSERT( !claim_cpu(MY_VICTIM_CPU) );
    ASSERT( !restore_system_state());
    print_system_settings();

    spawn_spy(MY_SPY_CPU, spy_func, 0);
    info_event("spy created; calling victim..");
    victim_func();
#else
    spy_func(0);
#endif

    info_event("collected %d measurements; discarded %d spurious IRQs", SAMPLES, spurious_irq);
    info("writing to '" DUMP_FILE "'");
    ASSERT( (fd = fopen(DUMP_FILE, "w")) );
    fprintf(fd, "tsc_diff,inc_count\n");
    for (int i = 0; i < SAMPLES; i++)
    {
        fprintf(fd, "%llu,%llu\n", test_tsc_results[i], test_inc_results[i]);
    }
    fclose(fd);

    info("all done; exiting");
    return 0;
}
