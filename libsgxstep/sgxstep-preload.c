#if !NO_SGX

#include <string.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <signal.h>
#include <Zydis.h>

#include "debug.h"
#include "elf_parser.h"
#include "pt.h"
#include "aep.h"
#include "enclave.h"
#include "sched.h"
#include "cpu.h"
#include "idt.h"
#include "apic.h"


#define DEBUG           1

#define INT3            0xcc
#define NOP             0x90
#define ENCLU_BYTES     "\x0f\x01\xd7"
#define ENCLU_LEN       3

#define MAX_PATCHED_SITES 128

idt_t idt = {0};

static void *patched_sites[MAX_PATCHED_SITES];
static int num_patched_sites = 0;
static const char *own_path = NULL;
int cnt = 0, tf_trap_cnt = 0;

static struct sigaction old_act = {0};

extern void eenter_stub(void);
extern void eresume_stub(void);
extern void *tcs_pt;
extern void sgx_step_aep_cb_func(void);
extern void sgx_step_aep_cb_func_placeholder(void);

extern void enclu_stub(void);
uint64_t *enclu_cont_instr;

// Settings
int tf_enabled = 0;
int tracer_enabled = 0;
int skip_ecalls = (int) ((unsigned int)~0 >> 1);
int target_ecall = -1;
int eenter_count = 0;

int sgx_step_do_irq = 0;

// Internal fns
static const char *get_own_path(void);

static void register_patched_site(void *addr)
{
    ASSERT( num_patched_sites < MAX_PATCHED_SITES );
    patched_sites[num_patched_sites++] = addr;
}

static int is_patched_enclu(void *addr)
{
    for (int i = 0; i < num_patched_sites; i++)
    {
        if (patched_sites[i] == addr) return 1;
    }
    
    return 0;
}

static void patch_enclu_in_region( uint64_t start, uint64_t end, char *pathname)
{
    // Give write permissions to the region
    uint64_t size = end - start;
    uint8_t *mem = (uint8_t *) start;

    ASSERT( !mprotect( (void*) (((uint64_t) start) & ~PFN_MASK), size, PROT_READ | PROT_EXEC  | PROT_WRITE));

    // find boundaries using zydis
    ZydisDecoder decoder;
    ZydisDecoderInit( &decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64 );

    uint64_t offset = 0;
    while( offset < size ) 
    {
        ZydisDecodedInstruction instr;
        ZydisDecoderContext ctx;	

	if(!ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, &ctx, mem + offset, size - offset, &instr)))
        {
            offset++;	
	    continue;
	}
	// Check length first to avoid memcmp on most instructions (short circuit)
        if(instr.length == ENCLU_LEN && memcmp(mem + offset, ENCLU_BYTES, ENCLU_LEN) == 0)
	{
	    #if DEBUG
                info("%s: patched enclu at %p: %02x %02x %02x", pathname, (void *) (start + offset), mem[offset], mem[offset + 1], mem[offset + 2]);
            #endif
		 
	    mem[offset] = INT3;
	    mem[offset + 1] = NOP;
	    mem[offset + 2] = NOP;
	    register_patched_site((void *) (start + offset));
	}

	offset += instr.length;
    }
    
}

/* Provoke page fault on enclave entry to initiate single-stepping mode. */
void attacker_config_page_table(void)
{
}

static void sgxstep_install_traps( void )
{
    
    uint64_t start, end = 0;
    char read, write, exec, cow;
    char *pathname = NULL;
    FILE *fd_self_maps;

    own_path = get_own_path();
    ASSERT((fd_self_maps = fopen("/proc/self/maps", "r")) >= 0);

    char line[512];
    while ( fgets(line, sizeof(line), fd_self_maps ) )
    { 
        // parse line
	sscanf(line, "%lx-%lx %c%c%c%c %*x %*x:%*x %*[0-9 ]%m[^\n]", 
	        &start, &end, &read, &write, &exec, &cow, &pathname);

        if ( pathname && strcmp(pathname, "[vsyscall]") == 0 ) continue;
        if ( pathname && own_path && strstr(pathname, "libsgx-step.so")) continue;
							    
        if (exec == 'x' && cow == 'p')	
	{
            #if DEBUG
                info("%p - %p xp %s", (void*) start, (void*) end, pathname);
            #endif
            patch_enclu_in_region(start, end, pathname);
        }

        free(pathname);
    } 

}

static const char *get_own_path(void)
{
    Dl_info info;
    dladdr((void *) sgxstep_install_traps , &info);
    return info.dli_fname;
}

__attribute__((visibility("default")))
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
    #if DEBUG
        info("======== Intercepted sigaction signo %d ========", signum);
    #endif
    static int (*real_sigaction)(int, const struct sigaction *, struct sigaction *) = NULL;
    if ( !real_sigaction )
        real_sigaction = dlsym(RTLD_NEXT, "sigaction");

    // Make them think they installed it (only care for sigtrap) 
    if ( signum == SIGTRAP ) 
    {
        memset(&old_act, 0, sizeof(sigaction));
        old_act = *act;
	return 0;
    }

    // If sigaction was called for other signal just forward
    return real_sigaction(signum, act, oldact);
}

/* Called upon SIGSEGV caused by untrusted page tables. */
void sgxstep_fault_handler(int signo, siginfo_t *si, void *ctx)
{
    #if DEBUG
        //info("Hello for libsgx-step.so fault handler...");
    #endif
    
    ucontext_t *uctx = (ucontext_t *) ctx;
    void *rip = (void *) uctx->uc_mcontext.gregs[REG_RIP];
    void *rax = (void *) uctx->uc_mcontext.gregs[REG_RAX];
    void *rbx = (void *) uctx->uc_mcontext.gregs[REG_RBX];
    void *rcx = (void *) uctx->uc_mcontext.gregs[REG_RCX];
    void *rdx = (void *) uctx->uc_mcontext.gregs[REG_RDX];
    

    switch( signo )
    {
        case SIGTRAP: 
            if (si->si_code == TRAP_TRACE)
            {
                /* TF flag caused SIGTRAP */ 
                #if DEBUG
                    uint64_t erip = edbgrd_erip() - (uint64_t) get_enclave_base();
                    info("^^ enclave RIP=%#llx; tf_trap_cnt = %d\n", erip, tf_trap_cnt++);
                #endif
                /* ensure RFLAGS.TF is clear to disable debug single-stepping */
                uctx->uc_mcontext.gregs[REG_EFL] &= ~0x100;
                break;
	    }
	    else if ( is_patched_enclu((void *)(rip - 1)) )
            {
                // Case eresume or eenter put the adress of AEP in and call enclu
		switch( (uint64_t) rax)
                {
                    case 2:  /* EENTER */
			eenter_count++;
			if (eenter_count > skip_ecalls || eenter_count == target_ecall || target_ecall == -2)
                        {				
			    info("eenter count %d, skip_ecalls %d, target_ecall %d", eenter_count, skip_ecalls, target_ecall);
                            if ( tf_enabled )
			    {
				info("tf flag set");
                                sgx_step_do_trap = 1;
			    }
		            else 
			    {
				info("irq is set");
				sgx_step_do_irq = 1;
			    }
		        }
			uctx->uc_mcontext.gregs[REG_RIP] = (uint64_t) &eenter_stub;
			enclu_cont_instr = rip;
			break;
                    case 3: /* ERESUME */
			uctx->uc_mcontext.gregs[REG_RIP] = (uint64_t) &eresume_stub;
			break;
                    default:
                        info("Unkown leaf enclu trap at %p leaf=%lu rcx:%p", (void *)(rip - 1), (uint64_t) rax, rcx); 
                        abort();
                }

                #if DEBUG
		    info("enclu trap %d at %p leaf=%lu rcx:%p", cnt++, (void *)(rip - 1), (uint64_t) rax, rcx); 
                #endif

                tcs_pt = (uint64_t *) rbx; 
		break;
            }
	    else
            {
               /*Not our signal :), double it and give it to the next person */
	       /* this allows for app defined sig handler as well :) */
               if (old_act.sa_flags & SA_SIGINFO)
               {
		   #if DEBUG
		       info("Not our signal, forward sa_sigaction");
		   #endif
                   // If SA_SIGINFO specified in flags sa_sigaction insteas of sa_handler
                   old_act.sa_sigaction(signo, si, ctx);
		   break;
               }
               else if (old_act.sa_handler == SIG_DFL)
               {
		   #if DEBUG
		       info("Not our signal, exec SIG_DFL");
		   #endif
		   /* No handler registered default behavior */
		   /* DLF behavior kills process, so we dont worry about lsgx-step handler */
		   struct sigaction dfl = {0};
		   dfl.sa_handler = SIG_DFL;
		   sigaction(SIGTRAP, &dfl, NULL);
		   raise(SIGTRAP);
		   break;
               }
	       else if (old_act.sa_handler == SIG_IGN)
	       {
		   #if DEBUG
		       info("Not our signal, exec SIG_IGN");
		   #endif
		   /* SIG_IGN, means supress the sigtrap */
		   break;
	       }
	       else
	       {
		   #if DEBUG
		       info("Not our signal, exec sa_handler");
		   #endif
	           /* App registered one arg handler */ 
                   old_act.sa_handler(signo);
		   break;
	       }
	    }
        default:
            info("Caught unexpected signal '%d'", signo);
	    abort();
    }
}

static void sgxstep_install_signal_handler( void )
{
    // Register the fault handler
    struct sigaction act, old_act;

    /* Specify #DB handler with signinfo arguments */
    memset(&act, 0, sizeof(sigaction));
    act.sa_sigaction = sgxstep_fault_handler;
    act.sa_flags = SA_RESTART | SA_SIGINFO;

    /* Block all signals while the signal is being handled */
    sigfillset(&act.sa_mask);

    /* get next symbol of sigaction in mem */
    int (*real_sigaction)(int, const struct sigaction *, struct sigaction *) = 
        dlsym(RTLD_NEXT, "sigaction");

    //ASSERT( !sigaction( SIGTRAP, &act, &old_act ));
    ASSERT( !real_sigaction( SIGTRAP, &act, &old_act ));
}

/* Configure and check attacker untrusted runtime environment. */
void attacker_config_runtime(void)
{
    ASSERT( !claim_cpu(VICTIM_CPU) );
    ASSERT( !prepare_system_for_benchmark(PSTATE_PCT) );
}

void setup_apic_timer( void )
{
    info_event("Establishing user-space APIC/IDT mappings");
    map_idt(&idt);
    install_kernel_irq_handler(&idt, __ss_irq_handler, IRQ_VECTOR);
    apic_timer_oneshot(IRQ_VECTOR);

    __ss_irq_fired = 0;
    info_event("Chekcing APIC timer IRQ");
    apic_timer_irq( SGX_STEP_TIMER_INTERVAL );
    while (!__ss_irq_fired);
    info("APIC timer IRQ handler seems to be working");
}

__attribute__((constructor)) void sgxstep_init( void )
{
    info("executing libsgx-step.so constructor");

    char *skip = getenv("SGXSTEP_SKIP_ECALL");
    char *target = getenv("SGXSTEP_TARGET_ECALL");
     
    if ( target ) 
    {
         target_ecall = atoi(target);
    } 
    else if ( skip )
    {
        skip_ecalls = atoi(skip);
    } 
    else 
    {
        // no option selected
        target_ecall = -2;
    }

    if (getenv("SGXSTEP_TF")) tf_enabled = 1;
    if (getenv("SGXSTEP_TRACER")) tracer_enabled = 1;

    // replace parse proc/self/maps enclu -> 0xcc nop nop
    sgxstep_install_traps();
    
    sgxstep_install_signal_handler();
    
    attacker_config_runtime();

    register_aep_cb(sgx_step_aep_cb_func_placeholder);
    //register_aep_cb(sgx_step_aep_cb_func);

    if (!tf_enabled) setup_apic_timer();
}

#if 0
__attribute__((destructor)) void sgxstep_destructor( void )
{
    info_event("Printing vcd...");
    sgx_tracer_vcd(&tracer, "trace.vcd");
    
    info_event("Destroying tracer...");
    sgx_tracer_destroy(&tracer); 
}
#endif

#endif
