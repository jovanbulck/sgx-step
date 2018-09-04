# A Practical Attack Framework for Precise Enclave Execution Control

<img src="logo.svg" width=160 alt="logo" align="left" />

This repository contains the source code of the SGX interrupt attack framework
accompanying our SysTEX 2017 [paper](systex17.pdf).

> Jo Van Bulck, Frank Piessens, and Raoul Strackx. 2017. SGX-Step: A Practical
> Attack Framework for Precise Enclave Execution Control. In Proceedings of the
> 2nd Workshop on System Software for Trusted Execution (SysTEX '17). 

SGX-Step is free software, licensed under
[GPLv3](https://www.gnu.org/licenses/gpl-3.0). The SGX-Step logo is derived
from Eadweard Muybridge's iconic [public
domain](https://en.wikipedia.org/wiki/Sallie_Gardner_at_a_Gallop) "Sallie
Gardner at a Gallop" photo series, which, like our enclave single-stepping
goal, breaks down the galloping horse dynamics into a series of individual
photo frames to reveal overall horse gait properties.

| SGX-Step release | Publication details                                                  | Comments 		         |
|------------------|----------------------------------------------------------------------|------------------------------|
| v1.1.0           | [ESSoS'18](https://people.cs.kuleuven.be/~jo.vanbulck/essos18.pdf)   | IA32 support. 		 |
| v1.0.0           | [SysTEX'17](https://people.cs.kuleuven.be/~jo.vanbulck/systex17.pdf) | Original SGX-Step framework. |

## Abstract

Protected module architectures such as Intel SGX hold the promise of protecting
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

## Overview

Crucial to the design of SGX-Step, as opposed to previous enclave preemption 
proposals, is the creation of user-space virtual memory mappings for physical
memory locations holding page table entries, as well as for the local x86 APIC
memory-mapped I/O configuration registers. This allows an untrusted,
attacker-controlled host process to easily (i) configure the APIC timer
one-shot/periodic interrupt source, (ii) trigger inter-processor interrupts,
and (iii) track or modify enclave page table entries directly from user-space.

![sgx-step-framework](https://user-images.githubusercontent.com/2464627/31863914-d6fbf5d8-b754-11e7-83e6-95e3c77dd0d6.png)

The above figure summarizes the sequence of hardware and software steps when
interrupting and resuming an SGX enclave through our framework.

1. The local APIC timer interrupt arrives within an enclaved instruction.
2. The processor executes the AEX procedure that securely stores execution
   context in the enclave’s SSA frame, initializes CPU registers, and vectors
   to the kernel-level interrupt handler.
3. Our `/dev/sgx-step` loadable kernel module registered itself in the APIC
   event call back list to make sure it is called on every timer interrupt. At
   this point, any attack-specific, kernel-level spy code can easily be plugged
   in. Furthermore, to enable precise evaluation of our approach on
   attacker-controlled debug enclaves, SGX-Step can *optionally* be instrumented
   to retrieve the stored instruction pointer from the interrupted enclave’s
   SSA frame using the `EDBGRD` instruction.
4. The kernel returns to the user space AEP trampoline. We modified the 
   untrusted runtime of the official SGX SDK to allow easy registration of a
   custom AEP stub.
5. At this point, any attack-specific user mode spy code can again easily be
   run, before the single-stepping adversary configures the APIC timer for the 
   next interrupt, just before executing (6) `ERESUME`.

## Building and Running

### 0. System Requirements

SGX-Step requires an [SGX-capable](https://github.com/ayeks/SGX-hardware) Intel
processor, and an off-the-shelf Linux kernel. Our evaluation was performed on
i7-6500U/6700 CPUs, running unmodified Linux versions 4.2.0/4.4.0. To make use
of SGX-Step's single-stepping features, the local APIC device needs to be
configured in memory-mapped xAPIC mode. The easiest way to do this is to pass
the `nox2apic` Linux [kernel
parameter](https://wiki.archlinux.org/index.php/Kernel_parameters) at boot
time. We furthermore advise passing the `iomem=relaxed`, `no_timer_check`, and
`isolcpus` parameters to respectively avoid too many warning messages in the
kernel logs and affinitize the victim process to an isolated CPU core.

```bash
$ sudo vim /etc/default/grub
  # GRUB_CMDLINE_LINUX_DEFAULT="quiet splash nox2apic iomem=relaxed no_timer_check isolcpus=1"
$ sudo update-grub && sudo reboot
```

Finally, in order to reproduce our experimental results, make sure to disable
C-States and SpeedStep technology in the BIOS configuration. The table below
lists currently supported Intel CPUs, together with their single-stepping APIC
timer interval.

| Model name            | CPU                                               | Base frequency | APIC timer interval |
|-----------------------|---------------------------------------------------|----------------|---------------------|
| Dell Latitude 7490    | [i7-8650U](https://ark.intel.com/products/124968) | 1.9 GHz        | 36                  |
| Dell Inspiron 13 7359 | [i7-6500U](https://ark.intel.com/products/88194)  | 2.5 GHz        | 25                  |
| Dell Optiplex 7040    | [i7-6700](https://ark.intel.com/products/88196)   | 3.4 GHz        | 19                  |
| Acer Aspire V15       | [i5-6200U](https://ark.intel.com/products/88193)  | 2.3 GHz        | 28                  |

### 1. Patch and install SGX SDK

To enable easy registration of a custom Asynchronous Exit Pointer (AEP) stub,
we modified the untrusted runtime of the official Intel SGX SDK. Proceed as
follows to checkout [linux-sgx](https://github.com/01org/linux-sgx) v2.1.1 and
apply our patches.

```bash
$ git submodule init
$ git submodule update
$ ./patch_sdk.sh
```

Now, follow the instructions in the
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
that points to an unmodified v2.1
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

For example, to build and run the `strlen` attack from the paper for a
benchmark enclave that processes the secret string 100 repeated times, execute:

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

## Using SGX-Step in your own projects

The easiest way to get started using the SGX-Step framwork in your own
projects, is through [git
submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules):

```bash
$ cd my/git/project
$ git submodule add git@github.com:jovanbulck/sgx-step.git
$ cd sgx-step # Now build `/dev/sgx-step` and `libsgxstep` as described above
```
Have a look at the Makefiles in the `app` directory to see how a client
application can link to `libsgxstep` plus any local SGX SDK/PSW packages.
