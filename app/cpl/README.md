Sample output:

```
[sched.c] continuing on CPU 1

--------------------------------------------------------------------------------
[main.c] Establishing user space GDT mapping
--------------------------------------------------------------------------------

[gdt.c] DTR.base=0xfffffe000002d000/size=127 (16 entries)
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[gdt.c] established user space GDT mapping at 0x7ff75335b000
[gdt.c] --------------------------------------------------------------------------------
[gdt.c] GDT[  0] @0x7ff75335b000=0x00000000 / 0x00000 (seg sel 0x00); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[  1] @0x7ff75335b008=0x00000000 / 0xfffff (seg sel 0x08); p=1; dpl=0; type=11; g=1
[gdt.c] GDT[  2] @0x7ff75335b010=0x00000000 / 0xfffff (seg sel 0x10); p=1; dpl=0; type=11; g=1
[gdt.c] GDT[  3] @0x7ff75335b018=0x00000000 / 0xfffff (seg sel 0x18); p=1; dpl=0; type= 3; g=1
[gdt.c] GDT[  4] @0x7ff75335b020=0x00000000 / 0xfffff (seg sel 0x23); p=1; dpl=3; type=11; g=1
[gdt.c] GDT[  5] @0x7ff75335b028=0x00000000 / 0xfffff (seg sel 0x2b); p=1; dpl=3; type= 3; g=1
[gdt.c] GDT[  6] @0x7ff75335b030=0x00000000 / 0xfffff (seg sel 0x33); p=1; dpl=3; type=11; g=1
[gdt.c] GDT[  7] @0x7ff75335b038=0x00000000 / 0x00000 (seg sel 0x38); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[  8] @0x7ff75335b040=0x0002f000 / 0x0206f (seg sel 0x40); p=1; dpl=0; type=11; g=0
[gdt.c] GDT[  9] @0x7ff75335b048=0x0000ffff / 0x0fe00 (seg sel 0x48); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 10] @0x7ff75335b050=0x00000000 / 0x00000 (seg sel 0x50); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 11] @0x7ff75335b058=0x00000000 / 0x00000 (seg sel 0x58); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 12] @0x7ff75335b060=0x00000000 / 0x00000 (seg sel 0x60); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 13] @0x7ff75335b068=0x00000000 / 0x00000 (seg sel 0x68); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 14] @0x7ff75335b070=0x00000000 / 0x00000 (seg sel 0x70); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 15] @0x7ff75335b078=0x00000000 / 0x00001 (seg sel 0x7b); p=1; dpl=3; type= 5; g=0
[gdt.c] --------------------------------------------------------------------------------

--------------------------------------------------------------------------------
[main.c] Installing and calling ring0 call gate
--------------------------------------------------------------------------------

[idt.c] IDT[ 13] @0x7ff75335b068 = 0x401284 (seg sel 0x10); p=1; dpl=3; type=12; ist=0
[main.c] back from call gate w CPL prev/cur=0/3; RDMSR=0xfee00800

--------------------------------------------------------------------------------
[main.c] Establishing user space IDT mapping
--------------------------------------------------------------------------------

[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7ff75335a000
[idt.c] --------------------------------------------------------------------------------
[idt.c] IDT[  0] @0x7ff75335a000 = 0xffffffff81a00dc0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  1] @0x7ff75335a010 = 0xffffffff81a013b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=3
[idt.c] IDT[  2] @0x7ff75335a020 = 0xffffffff81a018a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=2
[idt.c] IDT[  3] @0x7ff75335a030 = 0xffffffff81a01400 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] IDT[  4] @0x7ff75335a040 = 0xffffffff81a00e00 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] IDT[  5] @0x7ff75335a050 = 0xffffffff81a00e40 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  6] @0x7ff75335a060 = 0xffffffff81a00e80 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  7] @0x7ff75335a070 = 0xffffffff81a00ec0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  8] @0x7ff75335a080 = 0xffffffff81a00f00 (seg sel 0x10); p=1; dpl=0; type=14; ist=1
[idt.c] IDT[  9] @0x7ff75335a090 = 0xffffffff81a00f30 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 10] @0x7ff75335a0a0 = 0xffffffff81a00f70 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 11] @0x7ff75335a0b0 = 0xffffffff81a00fc0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 12] @0x7ff75335a0c0 = 0xffffffff81a01440 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 13] @0x7ff75335a0d0 = 0xffffffff81a01550 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 14] @0x7ff75335a0e0 = 0xffffffff81a015a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 15] @0x7ff75335a0f0 = 0xffffffff81a01010 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 16] @0x7ff75335a100 = 0xffffffff81a01050 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 17] @0x7ff75335a110 = 0xffffffff81a01090 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 18] @0x7ff75335a120 = 0xffffffff81a01640 (seg sel 0x10); p=1; dpl=0; type=14; ist=4
[idt.c] IDT[ 19] @0x7ff75335a130 = 0xffffffff81a010e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 20] @0x7ff75335a140 = 0xffffffff826a30b4 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 21] @0x7ff75335a150 = 0xffffffff826a30bd (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 22] @0x7ff75335a160 = 0xffffffff826a30c6 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 23] @0x7ff75335a170 = 0xffffffff826a30cf (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 24] @0x7ff75335a180 = 0xffffffff826a30d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 25] @0x7ff75335a190 = 0xffffffff826a30e1 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 26] @0x7ff75335a1a0 = 0xffffffff826a30ea (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 27] @0x7ff75335a1b0 = 0xffffffff826a30f3 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 28] @0x7ff75335a1c0 = 0xffffffff826a30fc (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 29] @0x7ff75335a1d0 = 0xffffffff826a3105 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 30] @0x7ff75335a1e0 = 0xffffffff826a310e (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 31] @0x7ff75335a1f0 = 0xffffffff826a3117 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 32] @0x7ff75335a200 = 0xffffffff81a00ab0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 33] @0x7ff75335a210 = 0xffffffff81a00218 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 34] @0x7ff75335a220 = 0xffffffff81a00220 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 35] @0x7ff75335a230 = 0xffffffff81a00228 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 36] @0x7ff75335a240 = 0xffffffff81a00230 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 37] @0x7ff75335a250 = 0xffffffff81a00238 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 38] @0x7ff75335a260 = 0xffffffff81a00240 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 39] @0x7ff75335a270 = 0xffffffff81a00248 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 40] @0x7ff75335a280 = 0xffffffff81a00250 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 41] @0x7ff75335a290 = 0xffffffff81a00258 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 42] @0x7ff75335a2a0 = 0xffffffff81a00260 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 43] @0x7ff75335a2b0 = 0xffffffff81a00268 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 44] @0x7ff75335a2c0 = 0xffffffff81a00270 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 45] @0x7ff75335a2d0 = 0x401269 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] IDT[ 46] @0x7ff75335a2e0 = 0xffffffff81a00280 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 47] @0x7ff75335a2f0 = 0xffffffff81a00288 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 48] @0x7ff75335a300 = 0xffffffff81a00290 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 49] @0x7ff75335a310 = 0xffffffff81a00298 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 50] @0x7ff75335a320 = 0xffffffff81a002a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 51] @0x7ff75335a330 = 0xffffffff81a002a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 52] @0x7ff75335a340 = 0xffffffff81a002b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 53] @0x7ff75335a350 = 0xffffffff81a002b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 54] @0x7ff75335a360 = 0xffffffff81a002c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 55] @0x7ff75335a370 = 0xffffffff81a002c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 56] @0x7ff75335a380 = 0xffffffff81a002d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 57] @0x7ff75335a390 = 0xffffffff81a002d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 58] @0x7ff75335a3a0 = 0xffffffff81a002e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 59] @0x7ff75335a3b0 = 0xffffffff81a002e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 60] @0x7ff75335a3c0 = 0xffffffff81a002f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 61] @0x7ff75335a3d0 = 0xffffffff81a002f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 62] @0x7ff75335a3e0 = 0xffffffff81a00300 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 63] @0x7ff75335a3f0 = 0xffffffff81a00308 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 64] @0x7ff75335a400 = 0xffffffff81a00310 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 65] @0x7ff75335a410 = 0xffffffff81a00318 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 66] @0x7ff75335a420 = 0xffffffff81a00320 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 67] @0x7ff75335a430 = 0xffffffff81a00328 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 68] @0x7ff75335a440 = 0xffffffff81a00330 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 69] @0x7ff75335a450 = 0xffffffff81a00338 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 70] @0x7ff75335a460 = 0xffffffff81a00340 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 71] @0x7ff75335a470 = 0xffffffff81a00348 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 72] @0x7ff75335a480 = 0xffffffff81a00350 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 73] @0x7ff75335a490 = 0xffffffff81a00358 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 74] @0x7ff75335a4a0 = 0xffffffff81a00360 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 75] @0x7ff75335a4b0 = 0xffffffff81a00368 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 76] @0x7ff75335a4c0 = 0xffffffff81a00370 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 77] @0x7ff75335a4d0 = 0xffffffff81a00378 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 78] @0x7ff75335a4e0 = 0xffffffff81a00380 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 79] @0x7ff75335a4f0 = 0xffffffff81a00388 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 80] @0x7ff75335a500 = 0xffffffff81a00390 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 81] @0x7ff75335a510 = 0xffffffff81a00398 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 82] @0x7ff75335a520 = 0xffffffff81a003a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 83] @0x7ff75335a530 = 0xffffffff81a003a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 84] @0x7ff75335a540 = 0xffffffff81a003b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 85] @0x7ff75335a550 = 0xffffffff81a003b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 86] @0x7ff75335a560 = 0xffffffff81a003c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 87] @0x7ff75335a570 = 0xffffffff81a003c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 88] @0x7ff75335a580 = 0xffffffff81a003d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 89] @0x7ff75335a590 = 0xffffffff81a003d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 90] @0x7ff75335a5a0 = 0xffffffff81a003e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 91] @0x7ff75335a5b0 = 0xffffffff81a003e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 92] @0x7ff75335a5c0 = 0xffffffff81a003f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 93] @0x7ff75335a5d0 = 0xffffffff81a003f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 94] @0x7ff75335a5e0 = 0xffffffff81a00400 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 95] @0x7ff75335a5f0 = 0xffffffff81a00408 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 96] @0x7ff75335a600 = 0xffffffff81a00410 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 97] @0x7ff75335a610 = 0xffffffff81a00418 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 98] @0x7ff75335a620 = 0xffffffff81a00420 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 99] @0x7ff75335a630 = 0xffffffff81a00428 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[100] @0x7ff75335a640 = 0xffffffff81a00430 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[101] @0x7ff75335a650 = 0xffffffff81a00438 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[102] @0x7ff75335a660 = 0xffffffff81a00440 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[103] @0x7ff75335a670 = 0xffffffff81a00448 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[104] @0x7ff75335a680 = 0xffffffff81a00450 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[105] @0x7ff75335a690 = 0xffffffff81a00458 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[106] @0x7ff75335a6a0 = 0xffffffff81a00460 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[107] @0x7ff75335a6b0 = 0xffffffff81a00468 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[108] @0x7ff75335a6c0 = 0xffffffff81a00470 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[109] @0x7ff75335a6d0 = 0xffffffff81a00478 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[110] @0x7ff75335a6e0 = 0xffffffff81a00480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[111] @0x7ff75335a6f0 = 0xffffffff81a00488 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[112] @0x7ff75335a700 = 0xffffffff81a00490 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[113] @0x7ff75335a710 = 0xffffffff81a00498 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[114] @0x7ff75335a720 = 0xffffffff81a004a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[115] @0x7ff75335a730 = 0xffffffff81a004a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[116] @0x7ff75335a740 = 0xffffffff81a004b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[117] @0x7ff75335a750 = 0xffffffff81a004b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[118] @0x7ff75335a760 = 0xffffffff81a004c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[119] @0x7ff75335a770 = 0xffffffff81a004c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[120] @0x7ff75335a780 = 0xffffffff81a004d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[121] @0x7ff75335a790 = 0xffffffff81a004d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[122] @0x7ff75335a7a0 = 0xffffffff81a004e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[123] @0x7ff75335a7b0 = 0xffffffff81a004e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[124] @0x7ff75335a7c0 = 0xffffffff81a004f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[125] @0x7ff75335a7d0 = 0xffffffff81a004f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[126] @0x7ff75335a7e0 = 0xffffffff81a00500 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[127] @0x7ff75335a7f0 = 0xffffffff81a00508 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[128] @0x7ff75335a800 = 0xffffffff81a01c00 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] IDT[129] @0x7ff75335a810 = 0xffffffff81a00518 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[130] @0x7ff75335a820 = 0xffffffff81a00520 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[131] @0x7ff75335a830 = 0xffffffff81a00528 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[132] @0x7ff75335a840 = 0xffffffff81a00530 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[133] @0x7ff75335a850 = 0xffffffff81a00538 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[134] @0x7ff75335a860 = 0xffffffff81a00540 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[135] @0x7ff75335a870 = 0xffffffff81a00548 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[136] @0x7ff75335a880 = 0xffffffff81a00550 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[137] @0x7ff75335a890 = 0xffffffff81a00558 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[138] @0x7ff75335a8a0 = 0xffffffff81a00560 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[139] @0x7ff75335a8b0 = 0xffffffff81a00568 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[140] @0x7ff75335a8c0 = 0xffffffff81a00570 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[141] @0x7ff75335a8d0 = 0xffffffff81a00578 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[142] @0x7ff75335a8e0 = 0xffffffff81a00580 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[143] @0x7ff75335a8f0 = 0xffffffff81a00588 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[144] @0x7ff75335a900 = 0xffffffff81a00590 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[145] @0x7ff75335a910 = 0xffffffff81a00598 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[146] @0x7ff75335a920 = 0xffffffff81a005a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[147] @0x7ff75335a930 = 0xffffffff81a005a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[148] @0x7ff75335a940 = 0xffffffff81a005b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[149] @0x7ff75335a950 = 0xffffffff81a005b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[150] @0x7ff75335a960 = 0xffffffff81a005c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[151] @0x7ff75335a970 = 0xffffffff81a005c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[152] @0x7ff75335a980 = 0xffffffff81a005d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[153] @0x7ff75335a990 = 0xffffffff81a005d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[154] @0x7ff75335a9a0 = 0xffffffff81a005e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[155] @0x7ff75335a9b0 = 0xffffffff81a005e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[156] @0x7ff75335a9c0 = 0xffffffff81a005f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[157] @0x7ff75335a9d0 = 0xffffffff81a005f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[158] @0x7ff75335a9e0 = 0xffffffff81a00600 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[159] @0x7ff75335a9f0 = 0xffffffff81a00608 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[160] @0x7ff75335aa00 = 0xffffffff81a00610 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[161] @0x7ff75335aa10 = 0xffffffff81a00618 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[162] @0x7ff75335aa20 = 0xffffffff81a00620 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[163] @0x7ff75335aa30 = 0xffffffff81a00628 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[164] @0x7ff75335aa40 = 0xffffffff81a00630 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[165] @0x7ff75335aa50 = 0xffffffff81a00638 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[166] @0x7ff75335aa60 = 0xffffffff81a00640 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[167] @0x7ff75335aa70 = 0xffffffff81a00648 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[168] @0x7ff75335aa80 = 0xffffffff81a00650 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[169] @0x7ff75335aa90 = 0xffffffff81a00658 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[170] @0x7ff75335aaa0 = 0xffffffff81a00660 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[171] @0x7ff75335aab0 = 0xffffffff81a00668 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[172] @0x7ff75335aac0 = 0xffffffff81a00670 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[173] @0x7ff75335aad0 = 0xffffffff81a00678 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[174] @0x7ff75335aae0 = 0xffffffff81a00680 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[175] @0x7ff75335aaf0 = 0xffffffff81a00688 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[176] @0x7ff75335ab00 = 0xffffffff81a00690 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[177] @0x7ff75335ab10 = 0xffffffff81a00698 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[178] @0x7ff75335ab20 = 0xffffffff81a006a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[179] @0x7ff75335ab30 = 0xffffffff81a006a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[180] @0x7ff75335ab40 = 0xffffffff81a006b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[181] @0x7ff75335ab50 = 0xffffffff81a006b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[182] @0x7ff75335ab60 = 0xffffffff81a006c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[183] @0x7ff75335ab70 = 0xffffffff81a006c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[184] @0x7ff75335ab80 = 0xffffffff81a006d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[185] @0x7ff75335ab90 = 0xffffffff81a006d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[186] @0x7ff75335aba0 = 0xffffffff81a006e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[187] @0x7ff75335abb0 = 0xffffffff81a006e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[188] @0x7ff75335abc0 = 0xffffffff81a006f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[189] @0x7ff75335abd0 = 0xffffffff81a006f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[190] @0x7ff75335abe0 = 0xffffffff81a00700 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[191] @0x7ff75335abf0 = 0xffffffff81a00708 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[192] @0x7ff75335ac00 = 0xffffffff81a00710 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[193] @0x7ff75335ac10 = 0xffffffff81a00718 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[194] @0x7ff75335ac20 = 0xffffffff81a00720 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[195] @0x7ff75335ac30 = 0xffffffff81a00728 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[196] @0x7ff75335ac40 = 0xffffffff81a00730 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[197] @0x7ff75335ac50 = 0xffffffff81a00738 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[198] @0x7ff75335ac60 = 0xffffffff81a00740 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[199] @0x7ff75335ac70 = 0xffffffff81a00748 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[200] @0x7ff75335ac80 = 0xffffffff81a00750 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[201] @0x7ff75335ac90 = 0xffffffff81a00758 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[202] @0x7ff75335aca0 = 0xffffffff81a00760 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[203] @0x7ff75335acb0 = 0xffffffff81a00768 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[204] @0x7ff75335acc0 = 0xffffffff81a00770 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[205] @0x7ff75335acd0 = 0xffffffff81a00778 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[206] @0x7ff75335ace0 = 0xffffffff81a00780 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[207] @0x7ff75335acf0 = 0xffffffff81a00788 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[208] @0x7ff75335ad00 = 0xffffffff81a00790 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[209] @0x7ff75335ad10 = 0xffffffff81a00798 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[210] @0x7ff75335ad20 = 0xffffffff81a007a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[211] @0x7ff75335ad30 = 0xffffffff81a007a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[212] @0x7ff75335ad40 = 0xffffffff81a007b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[213] @0x7ff75335ad50 = 0xffffffff81a007b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[214] @0x7ff75335ad60 = 0xffffffff81a007c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[215] @0x7ff75335ad70 = 0xffffffff81a007c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[216] @0x7ff75335ad80 = 0xffffffff81a007d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[217] @0x7ff75335ad90 = 0xffffffff81a007d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[218] @0x7ff75335ada0 = 0xffffffff81a007e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[219] @0x7ff75335adb0 = 0xffffffff81a007e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[220] @0x7ff75335adc0 = 0xffffffff81a007f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[221] @0x7ff75335add0 = 0xffffffff81a007f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[222] @0x7ff75335ade0 = 0xffffffff81a00800 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[223] @0x7ff75335adf0 = 0xffffffff81a00808 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[224] @0x7ff75335ae00 = 0xffffffff81a00810 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[225] @0x7ff75335ae10 = 0xffffffff81a00818 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[226] @0x7ff75335ae20 = 0xffffffff81a00820 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[227] @0x7ff75335ae30 = 0xffffffff81a00828 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[228] @0x7ff75335ae40 = 0xffffffff81a00830 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[229] @0x7ff75335ae50 = 0xffffffff81a00838 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[230] @0x7ff75335ae60 = 0xffffffff81a00840 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[231] @0x7ff75335ae70 = 0xffffffff81a00848 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[232] @0x7ff75335ae80 = 0xffffffff81a00850 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[233] @0x7ff75335ae90 = 0xffffffff81a00858 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[234] @0x7ff75335aea0 = 0xffffffff81a00860 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[235] @0x7ff75335aeb0 = 0xffffffff81a00868 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[236] @0x7ff75335aec0 = 0xffffffff81a00870 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[237] @0x7ff75335aed0 = 0xffffffff81a00878 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[238] @0x7ff75335aee0 = 0xffffffff81a01f70 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[239] @0x7ff75335aef0 = 0xffffffff81a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[240] @0x7ff75335af00 = 0xffffffff81a00cf0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[241] @0x7ff75335af10 = 0xffffffff81a00c60 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[242] @0x7ff75335af20 = 0xffffffff81a00bd0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[243] @0x7ff75335af30 = 0xffffffff81a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[244] @0x7ff75335af40 = 0xffffffff81a02120 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[245] @0x7ff75335af50 = 0xffffffff81a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[246] @0x7ff75335af60 = 0xffffffff81a02510 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[247] @0x7ff75335af70 = 0xffffffff81a02000 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[248] @0x7ff75335af80 = 0xffffffff81a00b40 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[249] @0x7ff75335af90 = 0xffffffff81a02090 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[250] @0x7ff75335afa0 = 0xffffffff81a021b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[251] @0x7ff75335afb0 = 0xffffffff81a02240 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[252] @0x7ff75335afc0 = 0xffffffff81a022d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[253] @0x7ff75335afd0 = 0xffffffff81a02360 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[254] @0x7ff75335afe0 = 0xffffffff81a023f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[255] @0x7ff75335aff0 = 0xffffffff81a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] --------------------------------------------------------------------------------

--------------------------------------------------------------------------------
[main.c] Installing and calling ring0 irq gate
--------------------------------------------------------------------------------

[idt.c] installed ring0 IRQ handler with target_rip=0x40128b
[idt.c] IDT[ 45] @0x7ff75335a2d0 = 0x40128b (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[main.c] back from irq gate w CPL prev/cur=0/3; RDMSR=0xfee00800
```
