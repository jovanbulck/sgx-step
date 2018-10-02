Sample output:

```
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[sched.c] continuing on CPU 1

--------------------------------------------------------------------------------
[main.c] Establishing user space GDT mapping
--------------------------------------------------------------------------------

[gdt.c] DTR.base=0xfffffe000002d000/size=127 (16 entries)
[gdt.c] established user space GDT mapping at 0x7f2e0b529000
[gdt.c] --------------------------------------------------------------------------------
[gdt.c] GDT[  0] @0x7f2e0b529000=0x00000000 / 0x00000 (seg sel 0x00); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[  1] @0x7f2e0b529008=0x00000000 / 0xfffff (seg sel 0x08); p=1; dpl=0; type=11; g=1
[gdt.c] GDT[  2] @0x7f2e0b529010=0x00000000 / 0xfffff (seg sel 0x10); p=1; dpl=0; type=11; g=1
[gdt.c] GDT[  3] @0x7f2e0b529018=0x00000000 / 0xfffff (seg sel 0x18); p=1; dpl=0; type= 3; g=1
[gdt.c] GDT[  4] @0x7f2e0b529020=0x00000000 / 0xfffff (seg sel 0x23); p=1; dpl=3; type=11; g=1
[gdt.c] GDT[  5] @0x7f2e0b529028=0x00000000 / 0xfffff (seg sel 0x2b); p=1; dpl=3; type= 3; g=1
[gdt.c] GDT[  6] @0x7f2e0b529030=0x00000000 / 0xfffff (seg sel 0x33); p=1; dpl=3; type=11; g=1
[gdt.c] GDT[  7] @0x7f2e0b529038=0x00000000 / 0x00000 (seg sel 0x38); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[  8] @0x7f2e0b529040=0x0002f000 / 0x0206f (seg sel 0x40); p=1; dpl=0; type=11; g=0
[gdt.c] GDT[  9] @0x7f2e0b529048=0x0000ffff / 0x0fe00 (seg sel 0x48); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 10] @0x7f2e0b529050=0x00000000 / 0x00000 (seg sel 0x50); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 11] @0x7f2e0b529058=0x00000000 / 0x00000 (seg sel 0x58); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 12] @0x7f2e0b529060=0x00000000 / 0x00000 (seg sel 0x60); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 13] @0x7f2e0b529068=0x00000000 / 0x00000 (seg sel 0x68); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 14] @0x7f2e0b529070=0x00000000 / 0x00000 (seg sel 0x70); p=0; dpl=0; type= 0; g=0
[gdt.c] GDT[ 15] @0x7f2e0b529078=0x00000000 / 0x00001 (seg sel 0x7b); p=1; dpl=3; type= 5; g=0
[gdt.c] --------------------------------------------------------------------------------

--------------------------------------------------------------------------------
[main.c] Establishing user space IDT mapping
--------------------------------------------------------------------------------

[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7f2e0b528000
[idt.c] installed IRQ handler with target_rip=0x400fcf
[idt.c] IDT[ 45] @0x7f2e0b5282d0 = 0x40363d (seg sel 0x33); p=1; dpl=3; type=15; ist=0
[idt.c] --------------------------------------------------------------------------------
[idt.c] IDT[  0] @0x7f2e0b528000 = 0xffffffffb5a00dc0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  1] @0x7f2e0b528010 = 0xffffffffb5a013b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=3
[idt.c] IDT[  2] @0x7f2e0b528020 = 0xffffffffb5a018a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=2
[idt.c] IDT[  3] @0x7f2e0b528030 = 0xffffffffb5a01400 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] IDT[  4] @0x7f2e0b528040 = 0xffffffffb5a00e00 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] IDT[  5] @0x7f2e0b528050 = 0xffffffffb5a00e40 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  6] @0x7f2e0b528060 = 0xffffffffb5a00e80 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  7] @0x7f2e0b528070 = 0xffffffffb5a00ec0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  8] @0x7f2e0b528080 = 0xffffffffb5a00f00 (seg sel 0x10); p=1; dpl=0; type=14; ist=1
[idt.c] IDT[  9] @0x7f2e0b528090 = 0xffffffffb5a00f30 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 10] @0x7f2e0b5280a0 = 0xffffffffb5a00f70 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 11] @0x7f2e0b5280b0 = 0xffffffffb5a00fc0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 12] @0x7f2e0b5280c0 = 0xffffffffb5a01440 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 13] @0x7f2e0b5280d0 = 0xffffffffb5a01550 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 14] @0x7f2e0b5280e0 = 0xffffffffb5a015a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 15] @0x7f2e0b5280f0 = 0xffffffffb5a01010 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 16] @0x7f2e0b528100 = 0xffffffffb5a01050 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 17] @0x7f2e0b528110 = 0xffffffffb5a01090 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 18] @0x7f2e0b528120 = 0xffffffffb5a01640 (seg sel 0x10); p=1; dpl=0; type=14; ist=4
[idt.c] IDT[ 19] @0x7f2e0b528130 = 0xffffffffb5a010e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 20] @0x7f2e0b528140 = 0xffffffffb66a20b4 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 21] @0x7f2e0b528150 = 0xffffffffb66a20bd (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 22] @0x7f2e0b528160 = 0xffffffffb66a20c6 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 23] @0x7f2e0b528170 = 0xffffffffb66a20cf (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 24] @0x7f2e0b528180 = 0xffffffffb66a20d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 25] @0x7f2e0b528190 = 0xffffffffb66a20e1 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 26] @0x7f2e0b5281a0 = 0xffffffffb66a20ea (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 27] @0x7f2e0b5281b0 = 0xffffffffb66a20f3 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 28] @0x7f2e0b5281c0 = 0xffffffffb66a20fc (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 29] @0x7f2e0b5281d0 = 0xffffffffb66a2105 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 30] @0x7f2e0b5281e0 = 0xffffffffb66a210e (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 31] @0x7f2e0b5281f0 = 0xffffffffb66a2117 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 32] @0x7f2e0b528200 = 0xffffffffb5a00ab0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 33] @0x7f2e0b528210 = 0xffffffffb5a00218 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 34] @0x7f2e0b528220 = 0xffffffffb5a00220 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 35] @0x7f2e0b528230 = 0xffffffffb5a00228 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 36] @0x7f2e0b528240 = 0xffffffffb5a00230 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 37] @0x7f2e0b528250 = 0xffffffffb5a00238 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 38] @0x7f2e0b528260 = 0xffffffffb5a00240 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 39] @0x7f2e0b528270 = 0xffffffffb5a00248 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 40] @0x7f2e0b528280 = 0xffffffffb5a00250 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 41] @0x7f2e0b528290 = 0xffffffffb5a00258 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 42] @0x7f2e0b5282a0 = 0xffffffffb5a00260 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 43] @0x7f2e0b5282b0 = 0xffffffffb5a00268 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 44] @0x7f2e0b5282c0 = 0xffffffffb5a00270 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 45] @0x7f2e0b5282d0 = 0x40363d (seg sel 0x33); p=1; dpl=3; type=15; ist=0
[idt.c] IDT[ 46] @0x7f2e0b5282e0 = 0xffffffffb5a00280 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 47] @0x7f2e0b5282f0 = 0xffffffffb5a00288 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 48] @0x7f2e0b528300 = 0xffffffffb5a00290 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 49] @0x7f2e0b528310 = 0xffffffffb5a00298 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 50] @0x7f2e0b528320 = 0xffffffffb5a002a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 51] @0x7f2e0b528330 = 0xffffffffb5a002a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 52] @0x7f2e0b528340 = 0xffffffffb5a002b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 53] @0x7f2e0b528350 = 0xffffffffb5a002b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 54] @0x7f2e0b528360 = 0xffffffffb5a002c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 55] @0x7f2e0b528370 = 0xffffffffb5a002c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 56] @0x7f2e0b528380 = 0xffffffffb5a002d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 57] @0x7f2e0b528390 = 0xffffffffb5a002d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 58] @0x7f2e0b5283a0 = 0xffffffffb5a002e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 59] @0x7f2e0b5283b0 = 0xffffffffb5a002e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 60] @0x7f2e0b5283c0 = 0xffffffffb5a002f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 61] @0x7f2e0b5283d0 = 0xffffffffb5a002f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 62] @0x7f2e0b5283e0 = 0xffffffffb5a00300 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 63] @0x7f2e0b5283f0 = 0xffffffffb5a00308 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 64] @0x7f2e0b528400 = 0xffffffffb5a00310 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 65] @0x7f2e0b528410 = 0xffffffffb5a00318 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 66] @0x7f2e0b528420 = 0xffffffffb5a00320 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 67] @0x7f2e0b528430 = 0xffffffffb5a00328 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 68] @0x7f2e0b528440 = 0xffffffffb5a00330 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 69] @0x7f2e0b528450 = 0xffffffffb5a00338 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 70] @0x7f2e0b528460 = 0xffffffffb5a00340 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 71] @0x7f2e0b528470 = 0xffffffffb5a00348 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 72] @0x7f2e0b528480 = 0xffffffffb5a00350 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 73] @0x7f2e0b528490 = 0xffffffffb5a00358 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 74] @0x7f2e0b5284a0 = 0xffffffffb5a00360 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 75] @0x7f2e0b5284b0 = 0xffffffffb5a00368 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 76] @0x7f2e0b5284c0 = 0xffffffffb5a00370 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 77] @0x7f2e0b5284d0 = 0xffffffffb5a00378 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 78] @0x7f2e0b5284e0 = 0xffffffffb5a00380 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 79] @0x7f2e0b5284f0 = 0xffffffffb5a00388 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 80] @0x7f2e0b528500 = 0xffffffffb5a00390 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 81] @0x7f2e0b528510 = 0xffffffffb5a00398 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 82] @0x7f2e0b528520 = 0xffffffffb5a003a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 83] @0x7f2e0b528530 = 0xffffffffb5a003a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 84] @0x7f2e0b528540 = 0xffffffffb5a003b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 85] @0x7f2e0b528550 = 0xffffffffb5a003b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 86] @0x7f2e0b528560 = 0xffffffffb5a003c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 87] @0x7f2e0b528570 = 0xffffffffb5a003c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 88] @0x7f2e0b528580 = 0xffffffffb5a003d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 89] @0x7f2e0b528590 = 0xffffffffb5a003d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 90] @0x7f2e0b5285a0 = 0xffffffffb5a003e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 91] @0x7f2e0b5285b0 = 0xffffffffb5a003e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 92] @0x7f2e0b5285c0 = 0xffffffffb5a003f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 93] @0x7f2e0b5285d0 = 0xffffffffb5a003f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 94] @0x7f2e0b5285e0 = 0xffffffffb5a00400 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 95] @0x7f2e0b5285f0 = 0xffffffffb5a00408 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 96] @0x7f2e0b528600 = 0xffffffffb5a00410 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 97] @0x7f2e0b528610 = 0xffffffffb5a00418 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 98] @0x7f2e0b528620 = 0xffffffffb5a00420 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 99] @0x7f2e0b528630 = 0xffffffffb5a00428 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[100] @0x7f2e0b528640 = 0xffffffffb5a00430 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[101] @0x7f2e0b528650 = 0xffffffffb5a00438 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[102] @0x7f2e0b528660 = 0xffffffffb5a00440 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[103] @0x7f2e0b528670 = 0xffffffffb5a00448 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[104] @0x7f2e0b528680 = 0xffffffffb5a00450 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[105] @0x7f2e0b528690 = 0xffffffffb5a00458 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[106] @0x7f2e0b5286a0 = 0xffffffffb5a00460 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[107] @0x7f2e0b5286b0 = 0xffffffffb5a00468 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[108] @0x7f2e0b5286c0 = 0xffffffffb5a00470 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[109] @0x7f2e0b5286d0 = 0xffffffffb5a00478 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[110] @0x7f2e0b5286e0 = 0xffffffffb5a00480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[111] @0x7f2e0b5286f0 = 0xffffffffb5a00488 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[112] @0x7f2e0b528700 = 0xffffffffb5a00490 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[113] @0x7f2e0b528710 = 0xffffffffb5a00498 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[114] @0x7f2e0b528720 = 0xffffffffb5a004a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[115] @0x7f2e0b528730 = 0xffffffffb5a004a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[116] @0x7f2e0b528740 = 0xffffffffb5a004b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[117] @0x7f2e0b528750 = 0xffffffffb5a004b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[118] @0x7f2e0b528760 = 0xffffffffb5a004c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[119] @0x7f2e0b528770 = 0xffffffffb5a004c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[120] @0x7f2e0b528780 = 0xffffffffb5a004d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[121] @0x7f2e0b528790 = 0xffffffffb5a004d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[122] @0x7f2e0b5287a0 = 0xffffffffb5a004e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[123] @0x7f2e0b5287b0 = 0xffffffffb5a004e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[124] @0x7f2e0b5287c0 = 0xffffffffb5a004f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[125] @0x7f2e0b5287d0 = 0xffffffffb5a004f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[126] @0x7f2e0b5287e0 = 0xffffffffb5a00500 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[127] @0x7f2e0b5287f0 = 0xffffffffb5a00508 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[128] @0x7f2e0b528800 = 0xffffffffb5a01c00 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] IDT[129] @0x7f2e0b528810 = 0xffffffffb5a00518 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[130] @0x7f2e0b528820 = 0xffffffffb5a00520 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[131] @0x7f2e0b528830 = 0xffffffffb5a00528 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[132] @0x7f2e0b528840 = 0xffffffffb5a00530 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[133] @0x7f2e0b528850 = 0xffffffffb5a00538 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[134] @0x7f2e0b528860 = 0xffffffffb5a00540 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[135] @0x7f2e0b528870 = 0xffffffffb5a00548 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[136] @0x7f2e0b528880 = 0xffffffffb5a00550 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[137] @0x7f2e0b528890 = 0xffffffffb5a00558 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[138] @0x7f2e0b5288a0 = 0xffffffffb5a00560 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[139] @0x7f2e0b5288b0 = 0xffffffffb5a00568 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[140] @0x7f2e0b5288c0 = 0xffffffffb5a00570 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[141] @0x7f2e0b5288d0 = 0xffffffffb5a00578 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[142] @0x7f2e0b5288e0 = 0xffffffffb5a00580 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[143] @0x7f2e0b5288f0 = 0xffffffffb5a00588 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[144] @0x7f2e0b528900 = 0xffffffffb5a00590 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[145] @0x7f2e0b528910 = 0xffffffffb5a00598 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[146] @0x7f2e0b528920 = 0xffffffffb5a005a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[147] @0x7f2e0b528930 = 0xffffffffb5a005a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[148] @0x7f2e0b528940 = 0xffffffffb5a005b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[149] @0x7f2e0b528950 = 0xffffffffb5a005b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[150] @0x7f2e0b528960 = 0xffffffffb5a005c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[151] @0x7f2e0b528970 = 0xffffffffb5a005c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[152] @0x7f2e0b528980 = 0xffffffffb5a005d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[153] @0x7f2e0b528990 = 0xffffffffb5a005d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[154] @0x7f2e0b5289a0 = 0xffffffffb5a005e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[155] @0x7f2e0b5289b0 = 0xffffffffb5a005e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[156] @0x7f2e0b5289c0 = 0xffffffffb5a005f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[157] @0x7f2e0b5289d0 = 0xffffffffb5a005f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[158] @0x7f2e0b5289e0 = 0xffffffffb5a00600 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[159] @0x7f2e0b5289f0 = 0xffffffffb5a00608 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[160] @0x7f2e0b528a00 = 0xffffffffb5a00610 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[161] @0x7f2e0b528a10 = 0xffffffffb5a00618 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[162] @0x7f2e0b528a20 = 0xffffffffb5a00620 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[163] @0x7f2e0b528a30 = 0xffffffffb5a00628 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[164] @0x7f2e0b528a40 = 0xffffffffb5a00630 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[165] @0x7f2e0b528a50 = 0xffffffffb5a00638 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[166] @0x7f2e0b528a60 = 0xffffffffb5a00640 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[167] @0x7f2e0b528a70 = 0xffffffffb5a00648 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[168] @0x7f2e0b528a80 = 0xffffffffb5a00650 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[169] @0x7f2e0b528a90 = 0xffffffffb5a00658 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[170] @0x7f2e0b528aa0 = 0xffffffffb5a00660 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[171] @0x7f2e0b528ab0 = 0xffffffffb5a00668 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[172] @0x7f2e0b528ac0 = 0xffffffffb5a00670 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[173] @0x7f2e0b528ad0 = 0xffffffffb5a00678 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[174] @0x7f2e0b528ae0 = 0xffffffffb5a00680 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[175] @0x7f2e0b528af0 = 0xffffffffb5a00688 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[176] @0x7f2e0b528b00 = 0xffffffffb5a00690 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[177] @0x7f2e0b528b10 = 0xffffffffb5a00698 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[178] @0x7f2e0b528b20 = 0xffffffffb5a006a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[179] @0x7f2e0b528b30 = 0xffffffffb5a006a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[180] @0x7f2e0b528b40 = 0xffffffffb5a006b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[181] @0x7f2e0b528b50 = 0xffffffffb5a006b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[182] @0x7f2e0b528b60 = 0xffffffffb5a006c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[183] @0x7f2e0b528b70 = 0xffffffffb5a006c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[184] @0x7f2e0b528b80 = 0xffffffffb5a006d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[185] @0x7f2e0b528b90 = 0xffffffffb5a006d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[186] @0x7f2e0b528ba0 = 0xffffffffb5a006e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[187] @0x7f2e0b528bb0 = 0xffffffffb5a006e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[188] @0x7f2e0b528bc0 = 0xffffffffb5a006f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[189] @0x7f2e0b528bd0 = 0xffffffffb5a006f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[190] @0x7f2e0b528be0 = 0xffffffffb5a00700 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[191] @0x7f2e0b528bf0 = 0xffffffffb5a00708 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[192] @0x7f2e0b528c00 = 0xffffffffb5a00710 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[193] @0x7f2e0b528c10 = 0xffffffffb5a00718 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[194] @0x7f2e0b528c20 = 0xffffffffb5a00720 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[195] @0x7f2e0b528c30 = 0xffffffffb5a00728 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[196] @0x7f2e0b528c40 = 0xffffffffb5a00730 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[197] @0x7f2e0b528c50 = 0xffffffffb5a00738 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[198] @0x7f2e0b528c60 = 0xffffffffb5a00740 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[199] @0x7f2e0b528c70 = 0xffffffffb5a00748 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[200] @0x7f2e0b528c80 = 0xffffffffb5a00750 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[201] @0x7f2e0b528c90 = 0xffffffffb5a00758 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[202] @0x7f2e0b528ca0 = 0xffffffffb5a00760 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[203] @0x7f2e0b528cb0 = 0xffffffffb5a00768 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[204] @0x7f2e0b528cc0 = 0xffffffffb5a00770 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[205] @0x7f2e0b528cd0 = 0xffffffffb5a00778 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[206] @0x7f2e0b528ce0 = 0xffffffffb5a00780 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[207] @0x7f2e0b528cf0 = 0xffffffffb5a00788 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[208] @0x7f2e0b528d00 = 0xffffffffb5a00790 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[209] @0x7f2e0b528d10 = 0xffffffffb5a00798 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[210] @0x7f2e0b528d20 = 0xffffffffb5a007a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[211] @0x7f2e0b528d30 = 0xffffffffb5a007a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[212] @0x7f2e0b528d40 = 0xffffffffb5a007b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[213] @0x7f2e0b528d50 = 0xffffffffb5a007b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[214] @0x7f2e0b528d60 = 0xffffffffb5a007c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[215] @0x7f2e0b528d70 = 0xffffffffb5a007c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[216] @0x7f2e0b528d80 = 0xffffffffb5a007d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[217] @0x7f2e0b528d90 = 0xffffffffb5a007d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[218] @0x7f2e0b528da0 = 0xffffffffb5a007e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[219] @0x7f2e0b528db0 = 0xffffffffb5a007e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[220] @0x7f2e0b528dc0 = 0xffffffffb5a007f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[221] @0x7f2e0b528dd0 = 0xffffffffb5a007f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[222] @0x7f2e0b528de0 = 0xffffffffb5a00800 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[223] @0x7f2e0b528df0 = 0xffffffffb5a00808 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[224] @0x7f2e0b528e00 = 0xffffffffb5a00810 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[225] @0x7f2e0b528e10 = 0xffffffffb5a00818 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[226] @0x7f2e0b528e20 = 0xffffffffb5a00820 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[227] @0x7f2e0b528e30 = 0xffffffffb5a00828 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[228] @0x7f2e0b528e40 = 0xffffffffb5a00830 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[229] @0x7f2e0b528e50 = 0xffffffffb5a00838 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[230] @0x7f2e0b528e60 = 0xffffffffb5a00840 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[231] @0x7f2e0b528e70 = 0xffffffffb5a00848 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[232] @0x7f2e0b528e80 = 0xffffffffb5a00850 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[233] @0x7f2e0b528e90 = 0xffffffffb5a00858 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[234] @0x7f2e0b528ea0 = 0xffffffffb5a00860 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[235] @0x7f2e0b528eb0 = 0xffffffffb5a00868 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[236] @0x7f2e0b528ec0 = 0xffffffffb5a00870 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[237] @0x7f2e0b528ed0 = 0xffffffffb5a00878 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[238] @0x7f2e0b528ee0 = 0xffffffffb5a01f70 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[239] @0x7f2e0b528ef0 = 0xffffffffb5a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[240] @0x7f2e0b528f00 = 0xffffffffb5a00cf0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[241] @0x7f2e0b528f10 = 0xffffffffb5a00c60 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[242] @0x7f2e0b528f20 = 0xffffffffb5a00bd0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[243] @0x7f2e0b528f30 = 0xffffffffb5a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[244] @0x7f2e0b528f40 = 0xffffffffb5a02120 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[245] @0x7f2e0b528f50 = 0xffffffffb5a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[246] @0x7f2e0b528f60 = 0xffffffffb5a02510 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[247] @0x7f2e0b528f70 = 0xffffffffb5a02000 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[248] @0x7f2e0b528f80 = 0xffffffffb5a00b40 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[249] @0x7f2e0b528f90 = 0xffffffffb5a02090 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[250] @0x7f2e0b528fa0 = 0xffffffffb5a021b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[251] @0x7f2e0b528fb0 = 0xffffffffb5a02240 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[252] @0x7f2e0b528fc0 = 0xffffffffb5a022d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[253] @0x7f2e0b528fd0 = 0xffffffffb5a02360 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[254] @0x7f2e0b528fe0 = 0xffffffffb5a023f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[255] @0x7f2e0b528ff0 = 0xffffffffb5a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] --------------------------------------------------------------------------------

--------------------------------------------------------------------------------
[main.c] Triggering user space software interrupts
--------------------------------------------------------------------------------


[main.c] ****** hello world from user space IRQ handler with count=0 ******
[file.c] reading buffer from '/dev/cpu/1/msr' (size=8)
[apic.c] established local memory mapping for APIC_BASE=0xfee00000 at 0x7f2e0b527000
[apic.c] APIC_ID=2000000; LVTT=400ef; TDCR=0
[main.c] APIC TPR/PPR is 0/224
[main.c] RSP at 0x7ffeac29b888
[main.c] RIP is 0x4012bc
[main.c] CS is 0x33
[main.c] EFLAGS is 0x202

[main.c] ****** hello world from user space IRQ handler with count=1 ******
[main.c] APIC TPR/PPR is 0/0
[main.c] RSP at 0x7ffeac29b888
[main.c] RIP is 0x4012be
[main.c] CS is 0x33
[main.c] EFLAGS is 0x202

--------------------------------------------------------------------------------
[main.c] Triggering user space APIC timer interrupts
--------------------------------------------------------------------------------

[apic.c] APIC timer one-shot mode with division 2 (lvtt=2d/tdcr=0)

[main.c] ****** hello world from user space IRQ handler with count=2 ******
[main.c] APIC TPR/PPR is 0/32
[main.c] RSP at 0x7ffeac29b888
[main.c] RIP is 0x401333
[main.c] CS is 0x33
[main.c] EFLAGS is 0x246
[main.c] returned from timer IRQ: flags=0x246

[main.c] ****** hello world from user space IRQ handler with count=3 ******
[main.c] APIC TPR/PPR is 0/32
[main.c] RSP at 0x7ffeac29b888
[main.c] RIP is 0x401333
[main.c] CS is 0x33
[main.c] EFLAGS is 0x246
[main.c] returned from timer IRQ: flags=0x246

[main.c] ****** hello world from user space IRQ handler with count=4 ******
[main.c] APIC TPR/PPR is 0/32
[main.c] RSP at 0x7ffeac29b888
[main.c] RIP is 0x401333
[main.c] CS is 0x33
[main.c] EFLAGS is 0x246
[main.c] returned from timer IRQ: flags=0x246
[apic.c] Restored APIC_LVTT=400ef/TDCR=0)
[file.c] writing buffer to '/dev/cpu/1/msr' (size=8)
[main.c] all is well; irq_handler_count=5; exiting..
```
