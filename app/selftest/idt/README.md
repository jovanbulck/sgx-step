Sample output:

```
[idt.c] locking IRQ handlers..
[sched.c] continuing on CPU 1

--------------------------------------------------------------------------------
[main.c] Installing and testing ring3 IDT handler
--------------------------------------------------------------------------------

[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[idt.c] established user space IDT mapping at 0x7fd245142000
[idt.c] installed asm IRQ handler at 33:0x55ffaccea000
[idt.c] IDT[ 45] @0x7fd2451422d0 = 0x55ffaccea000 (seg sel 0x33); p=1; dpl=3; type=15; ist=0

[main.c] Triggering ring3 software interrupts..
[main.c] returned from IRQ: my_cpl=3; irq_cpl=3; count=01; flags=0x246
[main.c] returned from IRQ: my_cpl=3; irq_cpl=3; count=02; flags=0x246
[main.c] returned from IRQ: my_cpl=3; irq_cpl=3; count=03; flags=0x246

[main.c] Triggering ring3 APIC timer interrupts..
[file.c] reading buffer from '/dev/cpu/1/msr' (size=8)
[apic.c] established local memory mapping for APIC_BASE=0xfee00000 at 0x7fd245141000
[apic.c] APIC_ID=2000000; LVTT=400ec; TDCR=0
[apic.c] APIC timer one-shot mode with division 2 (lvtt=2d/tdcr=0)
[main.c] returned from IRQ: my_cpl=3; irq_cpl=3; count=04; flags=0x246
[main.c] returned from IRQ: my_cpl=3; irq_cpl=3; count=05; flags=0x246
[main.c] returned from IRQ: my_cpl=3; irq_cpl=3; count=06; flags=0x246
[apic.c] Restored APIC_LVTT=400ec/TDCR=0)
[file.c] writing buffer to '/dev/cpu/1/msr' (size=8)

--------------------------------------------------------------------------------
[main.c] Installing and testing ring0 IDT handler
--------------------------------------------------------------------------------

[idt.c] installed asm IRQ handler at 10:0x55ffaccea000
[idt.c] IDT[ 45] @0x7fd2451422d0 = 0x55ffaccea000 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] installing and calling ring0 irq gate
[sched.c] continuing on CPU 1
[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7fd245140000
[idt.c] installed asm IRQ handler at 10:0x55ffaccea05a
[idt.c] IDT[ 49] @0x7fd245140310 = 0x55ffaccea05a (seg sel 0x10); p=1; dpl=3; type=15; ist=0
[main.c] back from exec_priv(pre_irq) with CPL=0

[main.c] Triggering ring3 software interrupts..
[main.c] returned from IRQ: my_cpl=3; irq_cpl=0; count=07; flags=0x246
[main.c] returned from IRQ: my_cpl=3; irq_cpl=0; count=08; flags=0x246
[main.c] returned from IRQ: my_cpl=3; irq_cpl=0; count=09; flags=0x246

[main.c] Triggering ring0 software interrupts..
[main.c] returned from IRQ: my_cpl=0; irq_cpl=0; count=10; flags=0x246
[main.c] returned from IRQ: my_cpl=0; irq_cpl=0; count=11; flags=0x246
[main.c] returned from IRQ: my_cpl=0; irq_cpl=0; count=12; flags=0x246

[main.c] Triggering ring3 APIC timer interrupts..
[apic.c] APIC timer one-shot mode with division 2 (lvtt=2d/tdcr=0)
[main.c] returned from IRQ: my_cpl=3; irq_cpl=0; count=13; flags=0x246
[main.c] returned from IRQ: my_cpl=3; irq_cpl=0; count=14; flags=0x246
[main.c] returned from IRQ: my_cpl=3; irq_cpl=0; count=15; flags=0x246

[main.c] Triggering ring0 APIC timer interrupts..
[main.c] returned from IRQ: my_cpl=0; irq_cpl=0; count=16; flags=0x246
[main.c] returned from IRQ: my_cpl=0; irq_cpl=0; count=17; flags=0x246
[main.c] returned from IRQ: my_cpl=0; irq_cpl=0; count=18; flags=0x246
[apic.c] Restored APIC_LVTT=400ec/TDCR=0)
[file.c] writing buffer to '/dev/cpu/1/msr' (size=8)
[main.c] all is well; irq_count=18; exiting..
```
