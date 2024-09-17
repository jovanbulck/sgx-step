Sample output:

```
[sched.c] continuing on CPU 1

--------------------------------------------------------------------------------
[main.c] Establishing user space GDT mapping
--------------------------------------------------------------------------------

[gdt.c] DTR.base=0xfffffe45202b3000/size=127 (16 entries)
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[gdt.c] established user space GDT mapping at 0x7f1924083000
[gdt.c] --------------------------------------------------------------------------------
[gdt.c] GDT[  0] @0x7f1924083000=0x00000000 / 0x00000 (seg sel 0x00); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[  1] @0x7f1924083008=0x00000000 / 0xfffff (seg sel 0x08); p=1; dpl=0; type=11; g=1
[gdt.c] GDT[  2] @0x7f1924083010=0x00000000 / 0xfffff (seg sel 0x10); p=1; dpl=0; type=11; g=1
[gdt.c] GDT[  3] @0x7f1924083018=0x00000000 / 0xfffff (seg sel 0x18); p=1; dpl=0; type= 3; g=1
[gdt.c] GDT[  4] @0x7f1924083020=0x00000000 / 0xfffff (seg sel 0x23); p=1; dpl=3; type=11; g=1
[gdt.c] GDT[  5] @0x7f1924083028=0x00000000 / 0xfffff (seg sel 0x2b); p=1; dpl=3; type= 3; g=1
[gdt.c] GDT[  6] @0x7f1924083030=0x00000000 / 0xfffff (seg sel 0x33); p=1; dpl=3; type=11; g=1
[gdt.c] GDT[  7] @0x7f1924083038=0x00000000 / 0x00000 (seg sel 0x38); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[  8] @0x7f1924083040=0x202b5000 / 0x04087 (seg sel 0x40); p=1; dpl=0; type=11; g=0
[gdt.c] GDT[  9] @0x7f1924083048=0x0000ffff / 0x0fe45 (seg sel 0x48); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 10] @0x7f1924083050=0x00000000 / 0x00000 (seg sel 0x50); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 11] @0x7f1924083058=0x00000000 / 0x00000 (seg sel 0x58); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 12] @0x7f1924083060=0x00000000 / 0x00000 (seg sel 0x60); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 13] @0x7f1924083068=0x00000000 / 0x00000 (seg sel 0x68); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 14] @0x7f1924083070=0x00000000 / 0x00000 (seg sel 0x70); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 15] @0x7f1924083078=0x00000000 / 0x00001 (seg sel 0x7b); p=1; dpl=3; type= 5; g=0
[gdt.c] --------------------------------------------------------------------------------

--------------------------------------------------------------------------------
[main.c] Installing and calling ring0 call gate
--------------------------------------------------------------------------------

[idt.c] IDT[ 13] @0x7f1924083068 = 0x557a76cc84bc (seg sel 0x10); p=1; dpl=3; type=12; ist=0
[main.c] back from call gate w CPL prev/cur=0/3; RDMSR=0x2

--------------------------------------------------------------------------------
[main.c] Establishing user space IDT mapping
--------------------------------------------------------------------------------

[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7f1924082000

--------------------------------------------------------------------------------
[main.c] Installing and calling ring0 irq gate
--------------------------------------------------------------------------------

[idt.c] installed asm IRQ handler at 10:0x557a76cc84c3
[idt.c] IDT[ 45] @0x7f19240822d0 = 0x557a76cc84c3 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[main.c] back from irq gate w CPL prev/cur=0/3; RDMSR=0x2

--------------------------------------------------------------------------------
[main.c] Calling ring0 function on user page with `exec_priv()`
--------------------------------------------------------------------------------

[idt.c] locking user-space IRQ gate handler page at 0x557a76ccb000
[idt.c] installing ring-0 IRQ gate
[sched.c] continuing on CPU 1
[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7f1924081000
[idt.c] installed asm IRQ handler at 10:0x557a76ccb000
[idt.c] IDT[ 49] @0x7f1924081310 = 0x557a76ccb000 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[main.c] back from my_ring0_func w CPL prev/cur=0/3; RDMSR=0x2
```
