> :warning: SGX-Step integration for Gramine is experimental, awaiting an abstract SDK interface for SGX-Step (cf. issue #28).

### Building the patched Gramine

0. First, make sure to build `libsgxstep.a` with the correct SSA framesize for Gramine:

```bash
$ cd ../../libsgxstep
$ GRAMINE=1 make clean all
```

1. Apply the patches in the untrusted Gramine runtime `host_entry.S` to be able to link to `libsgxstep.a`.

```bash
$ cd sdk/gramine/gramine
$ ../patch_entry.sh
```

2. Build the patched Gramine runtime and validate that the patches were properly applied in the modified Gramine loader.

```bash
$ meson setup build/ --buildtype=release -Dsgx=enabled
$ meson configure build/ -Dsgx=enabled
$ ninja -C build
$ objdump -d build/pal/src/host/linux-sgx/loader | grep sgx_set_aep
000000000000924c <sgx_set_aep>:
```

For Building with the out-of-tree (OOT) driver, in DEBUG mode:
```bash
$ meson setup build-debug/ --werror --buildtype=debug -Dsgx=enabled -Dsgx_driver=oot -Dsgx_driver_include_path=<sgx-step folder>/kernel/linux-sgx-driver
$ meson configure build-debug/ --werror --buildtype=debug -Dsgx=enabled -Dsgx_driver=oot -Dsgx_driver_include_path=<sgx-step folder>/kernel/linux-sgx-driver
```

You will also need to set:
```bash
$ sudo sysctl vm.mmap_min_addr=0
```
Prior to running anything in Gramine.


For Building with upstream drivers (Kernel >= 5.11). Note that these have not been tested with SGX-Step and may not work at the moment because Gramine uses the [VDSO](https://github.com/jovanbulck/sgx-step#0-system-requirements):

```bash
$ meson setup build-debug/ --werror --buildtype=debug -Dsgx=enabled -Dsgx_driver=upstream
$ meson configure build-debug/ --werror --buildtype=debug -Dsgx=enabled -Dsgx_driver=upstream
```

Note that only the OOT was tested on Ubuntu 20.04.4 LTS with Kernel 5.9 loaded. You may also have to experiment with the [kernel parameters](https://github.com/jovanbulck/sgx-step#0-system-requirements) before SGX-Step runs smoothly. The following worked on the tested configuration:

```bash
$ cat /proc/cmdline
... nox2apic iomem=relaxed no_timer_check nosmep nosmap clearcpuid=514 isolcpus=1 pti=off nmi_watchdog=0 rcuupdate.rcu_cpu_stall_suppress=1 msr.allow_writes=on intel_idle.max_cstate=1 processor.max_cstate=1 dis_ucode_ldr vt.handoff=7
```

3. Now, you can implement the required attack code in Gramine's untrusted
runtime using `libsgxstep` functionality as usual. For example, the following
patch in `host_ecalls.c` demonstrates some basic usages:

```bash
$ ./patch_ecall.sh
$ ninja -C build
$ objdump -d build/pal/src/host/linux-sgx/loader | grep sgx_step
   18631:	48 8d 05 2c 1c 00 00 	lea    0x1c2c(%rip),%rax        # 1a264 <sgx_step_aep_trampoline>
```

4. Install the patched Gramine.


```bash
$ sudo ninja -C build/ install
```
### Running a sample application with the patched Gramine and with the latency collection:

Proceed as follows. First Generate a private key:

```bash
$ export PYTHONPATH=/usr/local  # Fix https://github.com/gramineproject/gramine/issues/492
$ gramine-sgx-gen-private-key
```

The following changes are patched into the target application's manifest template with `0002-helloworld-Nemesis.patch`:

| Manifest setting                     | Motivation                                                         |
|--------------------------------------|------------------------------------------------------------------  |
|`loader.insecure__disable_aslr = true`      | For setting function offsets found through objdump in the binary                                                  | 
|`loader.insecure__use_cmdline_argv = true`  | For passing command line arguments to a binary                                                                    |
|`sgx.debug = true`                          | For getting debug output such as the offset of the libos and the binary                                           |
|`sgx.preheat_enclave = true`                | From my understanding of the [documentation](https://gramine.readthedocs.io/en/latest/manifest-syntax.html#pre-heating-enclave) this is equivalent to performing a dry run with the SDK            |
|`sgx.thread_num = 4`                        | This is probably optional, but Gramine needs 3 threads according to the [docs](https://gramine.readthedocs.io/en/latest/manifest-syntax.html#number-of-threads-deprecated-syntax), so we set it to 4 (assuming the binary is single-threaded) |


The example explicitly filters out all steps in the address range of the libOS (the assumption being that we are interested in the binary and not Gramine) and only logs stepping the `main()` function. The offsets of the LibOS and binary included in the Gramine patch will most likely not work. To retrieve them, execute `helloworld` in DEBUG mode. When determining the offset, it is recommended to turn off stepping (set `#define  SGX_STEP_ENABLE` to 0 in `host_ecalls.c`). Don't forget to run `ninja` after making changes to Gramine (and also have the sgx-step kernel module loaded).

```bash
$ cd CI-Examples/helloworld
$ make SGX=1 DEBUG=1
$ sudo gramine-sgx helloworld
```
Which will print plenty of debug output. For the libOS, you should see:

```
debug: LibOS loaded at <address>, ready to initialize
```
And for the `helloworld` binary:

```
[P1:T1:helloworld] debug: append_r_debug: adding file:helloworld at <address>
```

The addresses have to be replaced in `aep.c` (again, don't forget to run recompile libsgxstep and run `ninja`).

Now you can run the binary and watch it step through `main()`:

```bash
$ make clean && make SGX=1 DEBUG=1
$ sudo gramine-sgx ./helloworld > out
```

You may also want to pin it to the isolated core for less noise:
```bash
$ sudo taskset -c 1 gramine-sgx ./helloworld > out
```

Happy hacking!
