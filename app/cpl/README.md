Sample output:

```
[sched.c] continuing on CPU 1

--------------------------------------------------------------------------------
[main.c] Establishing user space GDT mapping
--------------------------------------------------------------------------------

[gdt.c] DTR.base=0xfffffe000002d000/size=127 (16 entries)
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[gdt.c] established user space GDT mapping at 0x7fe3a282c000
[gdt.c] --------------------------------------------------------------------------------
[gdt.c] GDT[  0] @0x7fe3a282c000=0x00000000 / 0x00000 (seg sel 0x00); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[  1] @0x7fe3a282c008=0x00000000 / 0xfffff (seg sel 0x08); p=1; dpl=0; type=11; g=1
[gdt.c] GDT[  2] @0x7fe3a282c010=0x00000000 / 0xfffff (seg sel 0x10); p=1; dpl=0; type=11; g=1
[gdt.c] GDT[  3] @0x7fe3a282c018=0x00000000 / 0xfffff (seg sel 0x18); p=1; dpl=0; type= 3; g=1
[gdt.c] GDT[  4] @0x7fe3a282c020=0x00000000 / 0xfffff (seg sel 0x23); p=1; dpl=3; type=11; g=1
[gdt.c] GDT[  5] @0x7fe3a282c028=0x00000000 / 0xfffff (seg sel 0x2b); p=1; dpl=3; type= 3; g=1
[gdt.c] GDT[  6] @0x7fe3a282c030=0x00000000 / 0xfffff (seg sel 0x33); p=1; dpl=3; type=11; g=1
[gdt.c] GDT[  7] @0x7fe3a282c038=0x00000000 / 0x00000 (seg sel 0x38); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[  8] @0x7fe3a282c040=0x0002f000 / 0x0206f (seg sel 0x40); p=1; dpl=0; type=11; g=0
[gdt.c] GDT[  9] @0x7fe3a282c048=0x0000ffff / 0x0fe00 (seg sel 0x48); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 10] @0x7fe3a282c050=0x00000000 / 0x00000 (seg sel 0x50); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 11] @0x7fe3a282c058=0x00000000 / 0x00000 (seg sel 0x58); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 12] @0x7fe3a282c060=0x00000000 / 0x00000 (seg sel 0x60); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 13] @0x7fe3a282c068=0x00000000 / 0x00000 (seg sel 0x68); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 14] @0x7fe3a282c070=0x00000000 / 0x00000 (seg sel 0x70); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 15] @0x7fe3a282c078=0x00000000 / 0x00001 (seg sel 0x7b); p=1; dpl=3; type= 5; g=0
[gdt.c] --------------------------------------------------------------------------------

--------------------------------------------------------------------------------
[main.c] Installing ring0 call gate
--------------------------------------------------------------------------------

[idt.c] IDT[ 13] @0x7fe3a282c068 = 0x40111d (seg sel 0x10); p=1; dpl=3; type=12; ist=0
[main.c] back from call gate w CPL prev/cur=0/3; RDMSR=0xfee00800
```
