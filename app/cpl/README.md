Sample output:

```
[sched.c] continuing on CPU 1

--------------------------------------------------------------------------------
[main.c] Establishing user space GDT mapping
--------------------------------------------------------------------------------

[gdt.c] DTR.base=0xfffffe000002c000/size=127 (16 entries)
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[gdt.c] established user space GDT mapping at 0x7f4ecd29f000

--------------------------------------------------------------------------------
[main.c] Installing and calling ring0 call gate
--------------------------------------------------------------------------------

[idt.c] IDT[ 13] @0x7f4ecd29f068 = 0x55921fa3340b (seg sel 0x10); p=1; dpl=3; type=12; ist=0
[main.c] back from call gate w CPL prev/cur=0/3; RDMSR=0xfee00800

--------------------------------------------------------------------------------
[main.c] Establishing user space IDT mapping
--------------------------------------------------------------------------------

[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7f4ecd29e000

--------------------------------------------------------------------------------
[main.c] Installing and calling ring0 irq gate
--------------------------------------------------------------------------------

[idt.c] installed ring0 IRQ handler with target_rip=0x55921fa33412
[idt.c] IDT[ 45] @0x7f4ecd29e2d0 = 0x55921fa33412 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[main.c] back from irq gate w CPL prev/cur=0/3; RDMSR=0xfee00800

--------------------------------------------------------------------------------
[main.c] Calling ring0 function on user page with `exec_priv()`
--------------------------------------------------------------------------------

[idt.c] installing and calling ring0 irq gate
[sched.c] continuing on CPU 1
[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7f4ecd29d000
[idt.c] installed ring0 IRQ handler with target_rip=0x55921fa35988
[idt.c] IDT[ 49] @0x7f4ecd29d310 = 0x55921fa35988 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[main.c] back from my_ring0_func w CPL=0 and IA32_TIME_STAMP_COUNTER=0x2e8755b4d06/0x2e8755b4e1e/280
```
