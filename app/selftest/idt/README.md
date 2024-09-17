Sample output:

```
[sched.c] continuing on CPU 1
[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[idt.c] established user space IDT mapping at 0x7f9a3aa22000

--------------------------------------------------------------------------------
[main.c] Installing and testing ring-0 IDT handler
--------------------------------------------------------------------------------

[idt.c] setting up ISR kernel mapping: 0x55d35036b000-0x55d35036c06a
[idt.c] received kernel base address 0xff7e259b00368000 with offset 0xff7dcfc7afffd000
[idt.c] using kernel-mapped ISR handler: 0x55d35036c000 -> 0xff7e259b00369000
[idt.c] installed asm IRQ handler at 10:0xff7e259b00369000
[idt.c] IDT[ 45] @0x7f9a3aa222d0 = 0xff7e259b00369000 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] locking user-space IRQ gate handler page at 0x55d350364000
[idt.c] installing ring-0 IRQ gate
[sched.c] continuing on CPU 1
[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7f9a3aa1f000
[idt.c] installed asm IRQ handler at 10:0x55d350364000
[idt.c] IDT[ 49] @0x7f9a3aa1f310 = 0x55d350364000 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[main.c] back from exec_priv(pre_irq) with CPL=0


--------------------------------------------------------------------------------
[main.c] Triggering ring-3 software interrupts..
--------------------------------------------------------------------------------

[main.c] returned from software IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=01; nemesis=2692
[main.c] returned from software IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=02; nemesis=400
[main.c] returned from software IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=03; nemesis=416
[main.c] returned from software IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=04; nemesis=414
[main.c] returned from software IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=05; nemesis=418
[main.c] returned from software IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=06; nemesis=416
[main.c] returned from software IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=07; nemesis=418
[main.c] returned from software IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=08; nemesis=414
[main.c] returned from software IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=09; nemesis=418
[main.c] returned from software IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=10; nemesis=410


--------------------------------------------------------------------------------
[main.c] Triggering ring-0 software interrupts..
--------------------------------------------------------------------------------

[main.c] returned from software IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=11; nemesis=350
[main.c] returned from software IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=12; nemesis=352
[main.c] returned from software IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=13; nemesis=354
[main.c] returned from software IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=14; nemesis=352
[main.c] returned from software IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=15; nemesis=352
[main.c] returned from software IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=16; nemesis=348
[main.c] returned from software IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=17; nemesis=350
[main.c] returned from software IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=18; nemesis=352
[main.c] returned from software IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=19; nemesis=350
[main.c] returned from software IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=20; nemesis=352
[idt.c] locking user-space IRQ gate handler page at 0x55d350364000
[idt.c] installing ring-0 IRQ gate
[sched.c] continuing on CPU 1
[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7f9a3aa1f000
[idt.c] installed asm IRQ handler at 10:0x55d35036400e
[idt.c] IDT[ 51] @0x7f9a3aa1f330 = 0x55d35036400e (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] locking user-space IRQ gate handler page at 0x55d350364000
[idt.c] installing ring-0 IRQ gate
[sched.c] continuing on CPU 1
[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7f9a3aa1f000
[idt.c] installed asm IRQ handler at 10:0x55d35036401f
[idt.c] IDT[ 50] @0x7f9a3aa1f320 = 0x55d35036401f (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[apic.c] APIC_ID=0; LVTT=0x400ec; TDCR=0
[apic.c] APIC timer one-shot mode with division 2 (lvtt=2d/tdcr=0)


--------------------------------------------------------------------------------
[main.c] Triggering ring-3 APIC timer oneshot interrupts..
--------------------------------------------------------------------------------

[main.c] returned from APIC timer oneshot IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=21; nemesis=3670
[main.c] returned from APIC timer oneshot IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=22; nemesis=3494
[main.c] returned from APIC timer oneshot IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=23; nemesis=3486
[main.c] returned from APIC timer oneshot IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=24; nemesis=3602
[main.c] returned from APIC timer oneshot IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=25; nemesis=3496
[main.c] returned from APIC timer oneshot IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=26; nemesis=3462
[main.c] returned from APIC timer oneshot IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=27; nemesis=3416
[main.c] returned from APIC timer oneshot IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=28; nemesis=3292
[main.c] returned from APIC timer oneshot IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=29; nemesis=3490
[main.c] returned from APIC timer oneshot IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=30; nemesis=3636


--------------------------------------------------------------------------------
[main.c] Triggering ring-0 APIC timer oneshot interrupts..
--------------------------------------------------------------------------------

[main.c] returned from APIC timer oneshot IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=31; nemesis=3276
[main.c] returned from APIC timer oneshot IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=32; nemesis=3338
[main.c] returned from APIC timer oneshot IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=33; nemesis=3212
[main.c] returned from APIC timer oneshot IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=34; nemesis=3152
[main.c] returned from APIC timer oneshot IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=35; nemesis=3386
[main.c] returned from APIC timer oneshot IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=36; nemesis=3338
[main.c] returned from APIC timer oneshot IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=37; nemesis=3360
[main.c] returned from APIC timer oneshot IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=38; nemesis=3186
[main.c] returned from APIC timer oneshot IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=39; nemesis=3344
[main.c] returned from APIC timer oneshot IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=40; nemesis=3318
[apic.c] APIC timer tsc-deadline mode (lvtt=4002d/tdcr=0)


--------------------------------------------------------------------------------
[main.c] Triggering ring-3 APIC timer tsc_deadline interrupts..
--------------------------------------------------------------------------------

[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=41; nemesis=1386
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=42; nemesis=1340
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=43; nemesis=1326
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=44; nemesis=1324
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=45; nemesis=1304
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=46; nemesis=1320
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=47; nemesis=1302
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=48; nemesis=1314
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=49; nemesis=1402
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=3; irq_cpl=0; my_flags=0x246; count=50; nemesis=1416


--------------------------------------------------------------------------------
[main.c] Triggering ring-0 APIC timer tsc_deadline interrupts..
--------------------------------------------------------------------------------

[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=51; nemesis=1928
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=52; nemesis=1808
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=53; nemesis=2138
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=54; nemesis=2030
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=55; nemesis=1866
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=56; nemesis=1898
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=57; nemesis=1956
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=58; nemesis=1882
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=59; nemesis=2058
[main.c] returned from APIC timer tsc_deadline IRQ: my_cpl=0; irq_cpl=0; my_flags=0x46; count=60; nemesis=1998
[main.c] all is well; irq_count=60; exiting..
```
