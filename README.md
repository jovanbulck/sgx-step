# A Practical Attack Framework for Precise Enclave Execution Control

<img src="logo.svg" width=160 alt="logo" align="left" />

SGX-Step is an open-source framework to facilitate side-channel attack research
on Intel x86 processors in general and Intel SGX platforms in particular.
SGX-Step consists of an adversarial Linux kernel driver and a small user-space
operating system library that allows to configure untrusted page table entries
and/or x86 APIC timer interrupts completely from user space. SGX-Step has been
leveraged in our own research, as well as by independent researchers, to enable
several new and improved enclaved execution attacks that gather side-channel
observations at a maximal temporal resolution (i.e., by interrupting the victim
enclave after _every_ single instruction).

**License.** SGX-Step is free software, licensed under
[GPLv3](https://www.gnu.org/licenses/gpl-3.0). The SGX-Step logo is derived
from Eadweard Muybridge's iconic [public
domain](https://en.wikipedia.org/wiki/Sallie_Gardner_at_a_Gallop) "Sallie
Gardner at a Gallop" photo series, which, like our enclave single-stepping
goal, breaks down the galloping horse dynamics into a series of individual
photo frames to reveal overall horse gait properties.

| SGX-Step release | Comments 		                                                              |
|------------------|----------------------------------------------------------------------------------|
| v1.4.0           | Privileged interrupt/call gates (Plundervolt).                                   |
| v1.3.0           | Transient-execution support (Foreshadow).                                        |
| v1.2.0           | User-space interrupt handling and deterministic zero-step filtering (Nemesis).   |
| v1.1.0           | IA32 support. 		                                                      |
| v1.0.0           | User-space page table manipulation and APIC timer single-stepping.               |

**Publications.** SGX-Step has been employed by several independent research
groups and has enabled a new line of high-resolution SGX attacks. A full
up-to-date list of known projects using SGX-Step is included at the
[bottom](#bottom) of this README. A copy of the original paper is available
[here](https://people.cs.kuleuven.be/~jo.vanbulck/systex17.pdf).

## Abstract

Trusted execution environments such as Intel SGX hold the promise of protecting
sensitive computations from a potentially compromised operating system. Recent
research convincingly demonstrated, however, that SGX's strengthened adversary
model also gives rise to to a new class of powerful, low-noise side-channel
attacks leveraging first-rate control over hardware. These attacks commonly
rely on frequent enclave preemptions to obtain fine-grained side-channel
observations. A maximal temporal resolution is achieved when the victim state
is measured after every instruction. Current state-of-the-art enclave
execution control schemes, however, do not generally achieve such
instruction-level granularity.

This paper presents SGX-Step, an open-source Linux kernel framework that allows
an untrusted host process to configure APIC timer interrupts and track page
table entries directly from user space. We contribute and evaluate an improved
approach to single-step enclaved execution at instruction-level granularity,
and we show how SGX-Step enables several new or improved attacks. Finally, we
discuss its implications for the design of effective defense mechanisms.

```
@inproceedings{vanbulck2017sgxstep,
    title={{SGX-Step}: A practical attack framework for precise enclave execution control},
    author = {Van Bulck, Jo and Piessens, Frank and Strackx, Raoul},
    booktitle = {2nd Workshop on System Software for Trusted Execution ({SysTEX} 2017)},
    month=Oct,
    organization = {{ACM}},
    year=2017,
    pages = {4:1--4:6},
}
```

## Overview

Crucial to the design of SGX-Step, as opposed to previous enclave preemption
proposals, is the creation of user-space virtual memory mappings for physical
memory locations holding page table entries, as well as for the local APIC
memory-mapped I/O configuration registers and the x86 Interrupt Descriptor
Table (IDT). This allows an untrusted, attacker-controlled host process to
easily (i) track or modify enclave page table entries, (ii) configure the APIC
timer one-shot/periodic interrupt source, (iii) trigger inter-processor
interrupts, and (iv) register custom interrupt handlers completely _within_
user space.

![sgx-step-framework](framework.png)

The above figure summarizes the sequence of hardware and software steps when
interrupting and resuming an SGX enclave through our framework.

1. The local APIC timer interrupt arrives within an enclaved instruction.
2. The processor executes the AEX procedure that securely stores execution
   context in the enclave’s SSA frame, initializes CPU registers, and vectors
   to the (user space) interrupt handler registered in the IDT.
3. At this point, any attack-specific, spy code can easily be plugged in.
4. The library returns to the user space AEP trampoline. We modified the
   untrusted runtime of the official SGX SDK to allow easy registration of a
   custom AEP stub. Furthermore, to enable precise evaluation of our approach on
   attacker-controlled benchmark debug enclaves, SGX-Step can *optionally* be
   instrumented to retrieve the stored instruction pointer from the interrupted
   enclave’s SSA frame. For this, our `/dev/sgx-step` driver offers an optional
   IOCTL call for the privileged `EDBGRD` instruction.
5. Thereafter, we configure the local APIC timer for the next interrupt
   by writing into the initial-count MMIO register, just before executing (6)
   `ERESUME`.

## Building and Running

### 0. System requirements

SGX-Step requires an [SGX-capable](https://github.com/ayeks/SGX-hardware) Intel
processor, and an off-the-shelf Linux kernel. Our evaluation was performed on
i7-6500U/6700 CPUs, running Ubuntu 18.04 with a stock Linux 4.15.0 kernel.
We summarize Linux [kernel parameters](https://www.kernel.org/doc/html/latest/admin-guide/kernel-parameters.html)
below.

| Linux kernel parameter           | Motivation                                                                                                                      |
|----------------------------------|------------------------------------------------------------------  |
| `nox2apic`                       | Configure local APIC device in memory-mapped I/O mode (to make use of SGX-Step's precise single-stepping features).   |
| `iomem=relaxed no_timer_check`   | Suppress unneeded warning messages in the kernel logs.             |
| `nmi_watchdog=0`                 | Suppress the kernel NMI watchdog.                                  |
| `isolcpus=1`                     | Affinitize the victim process to an isolated CPU core.             |
| `nosmap nosmep`                  | Disable Supervisor Mode Access/Execution Prevention (only when using SGX-Step's ring0 call gates).                  |
| `clearcpuid=514`                 | Disable User-Mode Instruction Prevention (on newer CPUs).          |
| `dis_ucode_ldr`                  | Optionally disable CPU microcode updates (recent transient-execution attack mitigations may necessitate re-calibrating the single-stepping interval).                  |

Pass the desired boot parameters to the kernel as follows:

```bash
$ sudo vim /etc/default/grub
  # Add the following line: GRUB_CMDLINE_LINUX_DEFAULT="quiet splash nox2apic iomem=relaxed no_timer_check nosmep nosmap clearcpuid=514 isolcpus=1 nmi_watchdog=0"
$ sudo update-grub && reboot
```

Finally, in order to reproduce our experimental results, make sure to disable
C-States and SpeedStep technology in the BIOS configuration.

### 1. Patch and install SGX SDK

To enable easy registration of a custom Asynchronous Exit Pointer (AEP) stub,
we modified the untrusted runtime of the official Intel SGX SDK. Proceed as
follows to checkout [linux-sgx](https://github.com/01org/linux-sgx) v2.11 and
apply our patches.

```bash
$ git submodule init
$ git submodule update
$ ./install_SGX_driver.sh              # tested on Ubuntu 18.04/20.04
$ ./install_SGX_SDK.sh                 # tested on Ubuntu 18.04/20.04
$ source /opt/intel/sgxsdk/environment # add to ~/.bashrc to preserve across terminal sessions
$ sudo service aesmd status            # stop/start aesmd service if needed
```

The above install scripts are tested on Ubuntu 18.04 LTS.
For other GNU/Linux distributions, please follow the instructions in the
[linux-sgx](https://github.com/01org/linux-sgx) project to build and install
the Intel SGX SDK and PSW packages. You will also need to build and load an
(unmodified) [linux-sgx-driver](https://github.com/01org/linux-sgx-driver) SGX
kernel module in order to use SGX-Step.

**Note (local installation).** The patched SGX SDK and PSW packages can be
installed locally, without affecting a compatible system-wide 'linux-sgx'
installation. For this, the example Makefiles support an `SGX_SDK`
environment variable that points to the local SDK installation directory. When
detecting a non-default SDK path (i.e., not `/opt/intel/sgxsdk`), the "run"
Makefile targets furthermore dynamically link against the patched
`libsgx_urts.so` untrusted runtime built in the local `linux-sgx` directory
(using the `LD_LIBRARY_PATH` environment variable).

**Note (32-bit support).** Instructions for building 32-bit versions of
the SGX SDK and SGX-Step can be found in [README-m32.md](README-m32.md).

### 2. Build and load `/dev/sgx-step`

SGX-Step comes with a loadable kernel module that exports an IOCTL interface to
the `libsgxstep` user-space library. The driver is mainly responsible for (i)
hooking the APIC timer interrupt handler, (ii) collecting untrusted page table
mappings, and optionally (iii) fetching the interrupted instruction pointer for
benchmark enclaves.

To build and load the `/dev/sgx-step` driver, execute:

```bash
$ cd kernel
$ make clean load
```

**Note (/dev/isgx).** Our driver uses some internal symbols and data structures
from the official Intel `/dev/isgx` driver. We therefore include a git submodule
that points to an unmodified v2.11
[linux-sgx-driver](https://github.com/intel/linux-sgx-driver).

**Note (/dev/mem).** We rely on Linux's virtual `/dev/mem` device to construct
user-level virtual memory mappings for APIC physical memory-mapped I/O
registers and page table entries of interest. Recent Linux distributions
typically enable the `CONFIG_STRICT_DEVMEM` option which prevents such use,
however. Our `/dev/sgx-step` driver therefore includes an
[approach](https://www.libcrack.so/2012/09/02/bypassing-devmem_is_allowed-with-kprobes/)
to bypass `devmem_is_allowed` checks, without having to recompile the kernel.

### 3. Build and run test applications

User-space applications can link to the `libsgxstep` library to make use of
SGX-Step's single-stepping and page table manipulation features. Have a look at
the example applications in the "app" directory.

For example, to build and run an elementary example application to test page
table manipulation features and SDK patches:

```bash
$ cd app/aep-redirect
$ make run
```

To test timer single-stepping functionality, try for example building and
running the `strlen` attack from the paper for a benchmark enclave that
processes the secret string 100 repeated times:

```bash
$ cd app/bench
$ NUM=100 STRLEN=1 make parse   # alternatively vary NUM and use BENCH=1 or ZIGZAG=1
$ # (above command defaults to the Dell Inspiron 13 7359 evaluation laptop machine;
$ # use DESKTOP=1 to build for a Dell Optiplex 7040 machine)
$ # use SGX_SDK=/home/jo/sgxsdk/ for a local SDK installation
$ # use M32=1 To produce a 32-bit executable
```

The above command builds `libsgxstep`, the benchmark victim enclave, and the
untrusted attacker host process, where the attack scenario and instance size
are configured via the corresponding environment variables. The same command
also runs the resulting binary non-interactively (to ensure deterministic timer
intervals), and finally calls an attack-specific post-processing Python script
to parse the resulting enclave instruction pointer benchmark results.

**Note (performance).** Single-stepping enclaved execution incurs a substantial
slowdown. We measured execution times of up to 15 minutes for the experiments
described in the paper. SGX-Step's page table manipulation features allow to
initiate single-stepping for selected functions only, for instance by revoking
access rights on specific code or data pages of interest.

**Note (timer interval).** The exact timer interval value depends on CPU
frequency, and hence remains inherently platform-specific. Configure a suitable
value in `/app/bench/main.c`. We established precise timer intervals for our
evaluation platforms (see table above) by tweaking and observing the NOP
microbenchmark enclave instruction pointer trace results.

#### Calibrating the single-stepping interval

The table below lists currently supported Intel CPUs, together with their
single-stepping APIC timer interval (`libsgxstep/config.h`).
Note that the exact single-stepping interval may depend on the microcode
version of the processor when recent [transient-execution attack](https://transient.fail/) mitigations are
in place to flush microarchitectural buffers on enclave entry/exit.
Some different microcode versions are provided for reference in the table below.

| Model name    | CPU                                               | Base frequency | ucode (date)      | APIC timer interval |
|---------------|---------------------------------------------------|----------------|-------------------|---------------------|
| Skylake       | [i7-6700](https://ark.intel.com/products/88196)   | 3.4 GHz        | ?                 | 19                  |
| Skylake       | [i7-6500U](https://ark.intel.com/products/88194)  | 2.5 GHz        | ?                 | 25                  |
| Skylake       | [i5-6200U](https://ark.intel.com/products/88193)  | 2.3 GHz        | ?                 | 28                  |
| Kaby Lake R   | [i7-8650U](https://ark.intel.com/products/124968) | 1.9 GHz        | ?                 | 34                  |
| Kaby Lake R   | [i7-8650U](https://ark.intel.com/products/124968) | 1.9 GHz        | 0xca (2019-10-03) | 54                  |
| Coffee Lake R	| [i9-9900K](https://ark.intel.com/products/186605) | 3.6 GHz        | ?                 | 21                  |
| Ice Lake      | [i5-1035G1](https://ark.intel.com/content/www/us/en/ark/products/196603/intel-core-i5-1035g1-processor-6m-cache-up-to-3-60-ghz.html) | 1.00 GHz  | 0x32 (2019-07-05) | 135 |

**Note (calibration).**
Currently, the easiest way to configure a reliable timer interval is to
use the `app/bench` benchmarking tool with a long NOP slide and
gradually increase/decrease `SGX_STEP_TIMER_INTERVAL`. You can probably
start around 20 and then execute `NUM=100 make parse` to get a summary
of single-steps, zero-steps, and multi-steps for a NOP slide of 100
instructions (once you have a more or less stable interval you can
switch to longer slides). Too many zero-steps indicate that you have to
increase the timer interval, whereas multi-steps demand lowering the
timer interval. Btw, don't worry when there's some zero-steps left, as
long as you make progress, you can always deterministically filter out
zero-steps by looking at the enclave's code PTE accessed bit (which is
only set when the instruction actually retires and a single-step occured).

**Note (microcode).**
Another word of caution relates to recent Foreshadow/ZombieLoad/RIDL/etc
microcode mitigations that flush leaky uarch buffers on enclave
entry/exit. Be aware that when these mitigations are enabled, the timer
interval will have to be increased as enclave entry takes longer (e.g.,
on my i7-8650U CPU I found the single-step timer interval goes up to 54
with recent ucode, from only 34 with pre-Foreshadow ucode).
The additional flushing operations may furthermore somewhat increase the
variance of enclave entry time, which implies that you might have to
configure the timer more conservatively with more zero-steps (which can be
deterministically filtered out as explained above).



## Using SGX-Step in your own projects

The easiest way to get started using the SGX-Step framwork in your own
projects, is through [git
submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules):

```bash
$ cd my/git/project
$ git submodule add https://github.com/jovanbulck/sgx-step.git
$ cd sgx-step # Now build `/dev/sgx-step` and `libsgxstep` as described above
```
Have a look at the Makefiles in the `app` directory to see how a client
application can link to `libsgxstep` plus any local SGX SDK/PSW packages.

<a name="bottom"></a>
The following is a list of known projects that use SGX-Step. Feel free to open
a pull request if your project uses SGX-Step but is not included below.

| Title | Publication details | Source code | SGX-Step features used |
|-------|---------------------|-------------|------------------------|
| CrossTalk: Speculative Data Leaks Across Cores Are Real | [S&P21](https://download.vusec.net/papers/crosstalk_sp21.pdf) | - | Single-stepping, page fault |
| Frontal Attack: Leaking Control-Flow in SGX via the CPU Frontend | [arXiv20](https://arxiv.org/pdf/2005.11516.pdf) | - | Single-stepping interrupt latency, PTE A/D |
| From A to Z: Projective coordinates leakage in the wild | [CHES20](https://eprint.iacr.org/2020/432.pdf) | - | Page fault |
| LVI: Hijacking Transient Execution through Microarchitectural Load Value Injection | [S&P20](https://lviattack.eu/lvi.pdf) | [link](https://github.com/jovanbulck/sgx-step-lvi/tree/master/app/lvi) | Single-stepping, page-table manipulation |
| CopyCat: Controlled Instruction-Level Attacks on Enclaves | [USEC20](https://arxiv.org/pdf/2002.08437.pdf) | - | Single-stepping, page fault, PTE A/D |
| When one vulnerable primitive turns viral: Novel single-trace attacks on ECDSA and RSA | [CHES20](https://eprint.iacr.org/2020/055.pdf) | - | Single-stepping, page fault, PTE A/D |
| Big Numbers - Big Troubles: Systematically Analyzing Nonce Leakage in (EC)DSA Implementations | [USEC20](https://www.usenix.org/system/files/sec20summer_weiser_prepub_0.pdf) | - | Page fault |
| Plundervolt: Software-based Fault Injection Attacks against Intel SGX | [S&P20](https://plundervolt.com/doc/plundervolt.pdf) | [link](https://github.com/KitMurdock/plundervolt) | Privileged interrupt/call gates, MSR |
| Bluethunder: A 2-level Directional Predictor Based Side-Channel Attack against SGX | [CHES20](https://heartever.github.io/files/bluethunder_sgx_ches.pdf) | - | Single-stepping |
| Fallout: Leaking Data on Meltdown-resistant CPUs | [CCS19](https://mdsattacks.com/files/fallout.pdf) | - | PTE A/D |
| A Tale of Two Worlds: Assessing the Vulnerability of Enclave Shielding Runtimes | [CCS19](https://people.cs.kuleuven.be/~jo.vanbulck/ccs19-tale.pdf) | [link](https://github.com/jovanbulck/0xbadc0de) | Single-stepping, page fault, PTE A/D |
| ZombieLoad: Cross-Privilege-Boundary Data Sampling | [CCS19](https://zombieloadattack.com/zombieload.pdf) | [link](https://github.com/IAIK/ZombieLoad/) | Single-stepping, zero-stepping, page-table manipulation |
| SPOILER: Speculative Load Hazards Boost Rowhammer and Cache Attacks | [USEC19](https://arxiv.org/pdf/1903.00446.pdf) | - | Single-stepping interrupt latency |
| Nemesis: Studying Microarchitectural Timing Leaks in Rudimentary CPU Interrupt Logic | [CCS18](https://people.cs.kuleuven.be/~jo.vanbulck/ccs18.pdf) | [link](https://github.com/jovanbulck/nemesis) | Single-stepping interrupt latency, page fault, PTE A/D |
| Foreshadow: Extracting the Keys to the Intel SGX Kingdom with Transient Out-of-Order Execution | [USEC18](https://foreshadowattack.eu/foreshadow.pdf) | [link](https://github.com/jovanbulck/sgx-step/tree/master/app/foreshadow) | Single-stepping, zero-stepping, page-table manipulation |
| Single Trace Attack Against RSA Key Generation in Intel SGX SSL | [AsiaCCS18](https://rspreitzer.github.io/publications/proc/asiaccs-2018-paper-1.pdf) | - | Page fault |
| Off-Limits: Abusing Legacy x86 Memory Segmentation to Spy on Enclaved Execution | [ESSoS18](https://people.cs.kuleuven.be/~jo.vanbulck/essos18.pdf) | [link](https://distrinet.cs.kuleuven.be/software/off-limits/) | Single-stepping, IA32 segmentation, page fault |
| SGX-Step: A Practical Attack Framework for Precise Enclave Execution Control | [SysTEX17](https://people.cs.kuleuven.be/~jo.vanbulck/systex17.pdf) | [link](https://github.com/jovanbulck/sgx-step/tree/master/app/bench) | Single-stepping, page fault, PTE A/D|
