# SGX-Step: A Practical Attack Framework for Precise Enclave Execution Control

This repository contains the source code of the SGX interrupt attack framework
accompanying our SysTEX 2017 [paper](systex17.pdf).

> Jo Van Bulck, Frank Piessens, and Raoul Strackx. 2017. SGX-Step: A Practical
> Attack Framework for Precise Enclave Execution Control. In Proceedings of the
> 2nd Workshop on System Software for Trusted Execution (SysTEX '17). 

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

## Building and Running

### 1. Patch and install SGX SDK

To enable easy registration of a custom Asynchronous Exit Pointer (AEP) stub,
we modified the untrusted runtime of the official Intel SGX SDK. Proceed as
follows to checkout [linux-sgx](https://github.com/01org/linux-sgx) v1.9 and
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
that points to an unmodified v1.9 [linux-sgx-driver](linux-sgx-driver).

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
benchmark enclave that processes the string 100 repeated times, execute:

```bash
$ cd app/bench/Enclave
$ make configure
$ cd ..
$ NUM=100 STRLEN=1 make parse   # alternatively vary NUM and use BENCH=1 or ZIGZAG=1
```

The above command builds `libsgxstep`, the benchmark victim enclave, and the
untrusted attacker host process, where the attack scenario and instance size
are configured via the corresponding environment variables. The same command
also runs the resulting binary non-interactively (to ensure deterministic timer
intervals), and finally calls and attack-specific post-processing Python script
to parse the resulting enclave instruction pointer benchmark results. The
Python scripts require the address of the benchmark instruction to be filled in
(can be easily retrieved with `objdump -D encl.so` after compilation).

**Note (performance).** Single-stepping enclaved execution incurs a substantial
slowdown. We measured execution times of up to 15 minutes for the experiments
described in the paper. SGX-Step's page table manipulation features allow to
initiate single-stepping for selected functions only, for instance by revoking
access rights on specific code or data pages of interest.

**Note (timer interval).** The exact timer interval value depends on CPU
frequency, and hence remains inherently platform-specific. Configure a suitable
value in `/app/bench/main.c`. We established precise timer intervals on both
our evaluation platforms by tweaking and observing the NOP microbenchmark
enclave instruction pointer trace results.

## License

SGX-Step is free software, licensed under [GPLv3](https://www.gnu.org/licenses/gpl-3.0).
