Sample output:

```
[sched.c] continuing on CPU 1

--------------------------------------------------------------------------------
[main.c] Establishing user space IDT mapping
--------------------------------------------------------------------------------

[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[idt.c] established user space IDT mapping at 0x7f9ba9160000
[idt.c] installed IRQ handler with target_rip=0x40101f
[idt.c] IDT[ 45] @0x7f9ba91602d0 = 0x403426 (seg sel 0x33); p=1; dpl=3; type=15; ist=0
[idt.c] --------------------------------------------------------------------------------
[idt.c] IDT[  0] @0x7f9ba9160000 = 0xffffffff81a00dc0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  1] @0x7f9ba9160010 = 0xffffffff81a013b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=3
[idt.c] IDT[  2] @0x7f9ba9160020 = 0xffffffff81a018a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=2
[idt.c] IDT[  3] @0x7f9ba9160030 = 0xffffffff81a01400 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] IDT[  4] @0x7f9ba9160040 = 0xffffffff81a00e00 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] IDT[  5] @0x7f9ba9160050 = 0xffffffff81a00e40 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  6] @0x7f9ba9160060 = 0xffffffff81a00e80 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  7] @0x7f9ba9160070 = 0xffffffff81a00ec0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[  8] @0x7f9ba9160080 = 0xffffffff81a00f00 (seg sel 0x10); p=1; dpl=0; type=14; ist=1
[idt.c] IDT[  9] @0x7f9ba9160090 = 0xffffffff81a00f30 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 10] @0x7f9ba91600a0 = 0xffffffff81a00f70 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 11] @0x7f9ba91600b0 = 0xffffffff81a00fc0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 12] @0x7f9ba91600c0 = 0xffffffff81a01440 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 13] @0x7f9ba91600d0 = 0xffffffff81a01550 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 14] @0x7f9ba91600e0 = 0xffffffff81a015a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 15] @0x7f9ba91600f0 = 0xffffffff81a01010 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 16] @0x7f9ba9160100 = 0xffffffff81a01050 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 17] @0x7f9ba9160110 = 0xffffffff81a01090 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 18] @0x7f9ba9160120 = 0xffffffff81a01640 (seg sel 0x10); p=1; dpl=0; type=14; ist=4
[idt.c] IDT[ 19] @0x7f9ba9160130 = 0xffffffff81a010e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 20] @0x7f9ba9160140 = 0xffffffff826a30b4 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 21] @0x7f9ba9160150 = 0xffffffff826a30bd (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 22] @0x7f9ba9160160 = 0xffffffff826a30c6 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 23] @0x7f9ba9160170 = 0xffffffff826a30cf (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 24] @0x7f9ba9160180 = 0xffffffff826a30d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 25] @0x7f9ba9160190 = 0xffffffff826a30e1 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 26] @0x7f9ba91601a0 = 0xffffffff826a30ea (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 27] @0x7f9ba91601b0 = 0xffffffff826a30f3 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 28] @0x7f9ba91601c0 = 0xffffffff826a30fc (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 29] @0x7f9ba91601d0 = 0xffffffff826a3105 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 30] @0x7f9ba91601e0 = 0xffffffff826a310e (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 31] @0x7f9ba91601f0 = 0xffffffff826a3117 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 32] @0x7f9ba9160200 = 0xffffffff81a00ab0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 33] @0x7f9ba9160210 = 0xffffffff81a00218 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 34] @0x7f9ba9160220 = 0xffffffff81a00220 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 35] @0x7f9ba9160230 = 0xffffffff81a00228 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 36] @0x7f9ba9160240 = 0xffffffff81a00230 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 37] @0x7f9ba9160250 = 0xffffffff81a00238 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 38] @0x7f9ba9160260 = 0xffffffff81a00240 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 39] @0x7f9ba9160270 = 0xffffffff81a00248 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 40] @0x7f9ba9160280 = 0xffffffff81a00250 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 41] @0x7f9ba9160290 = 0xffffffff81a00258 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 42] @0x7f9ba91602a0 = 0xffffffff81a00260 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 43] @0x7f9ba91602b0 = 0xffffffff81a00268 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 44] @0x7f9ba91602c0 = 0xffffffff81a00270 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 45] @0x7f9ba91602d0 = 0x403426 (seg sel 0x33); p=1; dpl=3; type=15; ist=0
[idt.c] IDT[ 46] @0x7f9ba91602e0 = 0xffffffff81a00280 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 47] @0x7f9ba91602f0 = 0xffffffff81a00288 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 48] @0x7f9ba9160300 = 0xffffffff81a00290 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 49] @0x7f9ba9160310 = 0xffffffff81a00298 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 50] @0x7f9ba9160320 = 0xffffffff81a002a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 51] @0x7f9ba9160330 = 0xffffffff81a002a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 52] @0x7f9ba9160340 = 0xffffffff81a002b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 53] @0x7f9ba9160350 = 0xffffffff81a002b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 54] @0x7f9ba9160360 = 0xffffffff81a002c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 55] @0x7f9ba9160370 = 0xffffffff81a002c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 56] @0x7f9ba9160380 = 0xffffffff81a002d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 57] @0x7f9ba9160390 = 0xffffffff81a002d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 58] @0x7f9ba91603a0 = 0xffffffff81a002e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 59] @0x7f9ba91603b0 = 0xffffffff81a002e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 60] @0x7f9ba91603c0 = 0xffffffff81a002f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 61] @0x7f9ba91603d0 = 0xffffffff81a002f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 62] @0x7f9ba91603e0 = 0xffffffff81a00300 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 63] @0x7f9ba91603f0 = 0xffffffff81a00308 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 64] @0x7f9ba9160400 = 0xffffffff81a00310 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 65] @0x7f9ba9160410 = 0xffffffff81a00318 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 66] @0x7f9ba9160420 = 0xffffffff81a00320 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 67] @0x7f9ba9160430 = 0xffffffff81a00328 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 68] @0x7f9ba9160440 = 0xffffffff81a00330 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 69] @0x7f9ba9160450 = 0xffffffff81a00338 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 70] @0x7f9ba9160460 = 0xffffffff81a00340 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 71] @0x7f9ba9160470 = 0xffffffff81a00348 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 72] @0x7f9ba9160480 = 0xffffffff81a00350 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 73] @0x7f9ba9160490 = 0xffffffff81a00358 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 74] @0x7f9ba91604a0 = 0xffffffff81a00360 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 75] @0x7f9ba91604b0 = 0xffffffff81a00368 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 76] @0x7f9ba91604c0 = 0xffffffff81a00370 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 77] @0x7f9ba91604d0 = 0xffffffff81a00378 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 78] @0x7f9ba91604e0 = 0xffffffff81a00380 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 79] @0x7f9ba91604f0 = 0xffffffff81a00388 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 80] @0x7f9ba9160500 = 0xffffffff81a00390 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 81] @0x7f9ba9160510 = 0xffffffff81a00398 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 82] @0x7f9ba9160520 = 0xffffffff81a003a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 83] @0x7f9ba9160530 = 0xffffffff81a003a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 84] @0x7f9ba9160540 = 0xffffffff81a003b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 85] @0x7f9ba9160550 = 0xffffffff81a003b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 86] @0x7f9ba9160560 = 0xffffffff81a003c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 87] @0x7f9ba9160570 = 0xffffffff81a003c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 88] @0x7f9ba9160580 = 0xffffffff81a003d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 89] @0x7f9ba9160590 = 0xffffffff81a003d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 90] @0x7f9ba91605a0 = 0xffffffff81a003e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 91] @0x7f9ba91605b0 = 0xffffffff81a003e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 92] @0x7f9ba91605c0 = 0xffffffff81a003f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 93] @0x7f9ba91605d0 = 0xffffffff81a003f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 94] @0x7f9ba91605e0 = 0xffffffff81a00400 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 95] @0x7f9ba91605f0 = 0xffffffff81a00408 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 96] @0x7f9ba9160600 = 0xffffffff81a00410 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 97] @0x7f9ba9160610 = 0xffffffff81a00418 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 98] @0x7f9ba9160620 = 0xffffffff81a00420 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[ 99] @0x7f9ba9160630 = 0xffffffff81a00428 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[100] @0x7f9ba9160640 = 0xffffffff81a00430 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[101] @0x7f9ba9160650 = 0xffffffff81a00438 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[102] @0x7f9ba9160660 = 0xffffffff81a00440 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[103] @0x7f9ba9160670 = 0xffffffff81a00448 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[104] @0x7f9ba9160680 = 0xffffffff81a00450 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[105] @0x7f9ba9160690 = 0xffffffff81a00458 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[106] @0x7f9ba91606a0 = 0xffffffff81a00460 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[107] @0x7f9ba91606b0 = 0xffffffff81a00468 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[108] @0x7f9ba91606c0 = 0xffffffff81a00470 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[109] @0x7f9ba91606d0 = 0xffffffff81a00478 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[110] @0x7f9ba91606e0 = 0xffffffff81a00480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[111] @0x7f9ba91606f0 = 0xffffffff81a00488 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[112] @0x7f9ba9160700 = 0xffffffff81a00490 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[113] @0x7f9ba9160710 = 0xffffffff81a00498 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[114] @0x7f9ba9160720 = 0xffffffff81a004a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[115] @0x7f9ba9160730 = 0xffffffff81a004a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[116] @0x7f9ba9160740 = 0xffffffff81a004b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[117] @0x7f9ba9160750 = 0xffffffff81a004b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[118] @0x7f9ba9160760 = 0xffffffff81a004c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[119] @0x7f9ba9160770 = 0xffffffff81a004c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[120] @0x7f9ba9160780 = 0xffffffff81a004d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[121] @0x7f9ba9160790 = 0xffffffff81a004d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[122] @0x7f9ba91607a0 = 0xffffffff81a004e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[123] @0x7f9ba91607b0 = 0xffffffff81a004e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[124] @0x7f9ba91607c0 = 0xffffffff81a004f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[125] @0x7f9ba91607d0 = 0xffffffff81a004f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[126] @0x7f9ba91607e0 = 0xffffffff81a00500 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[127] @0x7f9ba91607f0 = 0xffffffff81a00508 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[128] @0x7f9ba9160800 = 0xffffffff81a01c00 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[idt.c] IDT[129] @0x7f9ba9160810 = 0xffffffff81a00518 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[130] @0x7f9ba9160820 = 0xffffffff81a00520 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[131] @0x7f9ba9160830 = 0xffffffff81a00528 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[132] @0x7f9ba9160840 = 0xffffffff81a00530 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[133] @0x7f9ba9160850 = 0xffffffff81a00538 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[134] @0x7f9ba9160860 = 0xffffffff81a00540 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[135] @0x7f9ba9160870 = 0xffffffff81a00548 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[136] @0x7f9ba9160880 = 0xffffffff81a00550 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[137] @0x7f9ba9160890 = 0xffffffff81a00558 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[138] @0x7f9ba91608a0 = 0xffffffff81a00560 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[139] @0x7f9ba91608b0 = 0xffffffff81a00568 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[140] @0x7f9ba91608c0 = 0xffffffff81a00570 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[141] @0x7f9ba91608d0 = 0xffffffff81a00578 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[142] @0x7f9ba91608e0 = 0xffffffff81a00580 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[143] @0x7f9ba91608f0 = 0xffffffff81a00588 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[144] @0x7f9ba9160900 = 0xffffffff81a00590 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[145] @0x7f9ba9160910 = 0xffffffff81a00598 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[146] @0x7f9ba9160920 = 0xffffffff81a005a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[147] @0x7f9ba9160930 = 0xffffffff81a005a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[148] @0x7f9ba9160940 = 0xffffffff81a005b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[149] @0x7f9ba9160950 = 0xffffffff81a005b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[150] @0x7f9ba9160960 = 0xffffffff81a005c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[151] @0x7f9ba9160970 = 0xffffffff81a005c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[152] @0x7f9ba9160980 = 0xffffffff81a005d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[153] @0x7f9ba9160990 = 0xffffffff81a005d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[154] @0x7f9ba91609a0 = 0xffffffff81a005e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[155] @0x7f9ba91609b0 = 0xffffffff81a005e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[156] @0x7f9ba91609c0 = 0xffffffff81a005f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[157] @0x7f9ba91609d0 = 0xffffffff81a005f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[158] @0x7f9ba91609e0 = 0xffffffff81a00600 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[159] @0x7f9ba91609f0 = 0xffffffff81a00608 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[160] @0x7f9ba9160a00 = 0xffffffff81a00610 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[161] @0x7f9ba9160a10 = 0xffffffff81a00618 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[162] @0x7f9ba9160a20 = 0xffffffff81a00620 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[163] @0x7f9ba9160a30 = 0xffffffff81a00628 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[164] @0x7f9ba9160a40 = 0xffffffff81a00630 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[165] @0x7f9ba9160a50 = 0xffffffff81a00638 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[166] @0x7f9ba9160a60 = 0xffffffff81a00640 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[167] @0x7f9ba9160a70 = 0xffffffff81a00648 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[168] @0x7f9ba9160a80 = 0xffffffff81a00650 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[169] @0x7f9ba9160a90 = 0xffffffff81a00658 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[170] @0x7f9ba9160aa0 = 0xffffffff81a00660 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[171] @0x7f9ba9160ab0 = 0xffffffff81a00668 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[172] @0x7f9ba9160ac0 = 0xffffffff81a00670 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[173] @0x7f9ba9160ad0 = 0xffffffff81a00678 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[174] @0x7f9ba9160ae0 = 0xffffffff81a00680 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[175] @0x7f9ba9160af0 = 0xffffffff81a00688 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[176] @0x7f9ba9160b00 = 0xffffffff81a00690 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[177] @0x7f9ba9160b10 = 0xffffffff81a00698 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[178] @0x7f9ba9160b20 = 0xffffffff81a006a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[179] @0x7f9ba9160b30 = 0xffffffff81a006a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[180] @0x7f9ba9160b40 = 0xffffffff81a006b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[181] @0x7f9ba9160b50 = 0xffffffff81a006b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[182] @0x7f9ba9160b60 = 0xffffffff81a006c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[183] @0x7f9ba9160b70 = 0xffffffff81a006c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[184] @0x7f9ba9160b80 = 0xffffffff81a006d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[185] @0x7f9ba9160b90 = 0xffffffff81a006d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[186] @0x7f9ba9160ba0 = 0xffffffff81a006e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[187] @0x7f9ba9160bb0 = 0xffffffff81a006e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[188] @0x7f9ba9160bc0 = 0xffffffff81a006f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[189] @0x7f9ba9160bd0 = 0xffffffff81a006f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[190] @0x7f9ba9160be0 = 0xffffffff81a00700 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[191] @0x7f9ba9160bf0 = 0xffffffff81a00708 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[192] @0x7f9ba9160c00 = 0xffffffff81a00710 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[193] @0x7f9ba9160c10 = 0xffffffff81a00718 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[194] @0x7f9ba9160c20 = 0xffffffff81a00720 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[195] @0x7f9ba9160c30 = 0xffffffff81a00728 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[196] @0x7f9ba9160c40 = 0xffffffff81a00730 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[197] @0x7f9ba9160c50 = 0xffffffff81a00738 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[198] @0x7f9ba9160c60 = 0xffffffff81a00740 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[199] @0x7f9ba9160c70 = 0xffffffff81a00748 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[200] @0x7f9ba9160c80 = 0xffffffff81a00750 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[201] @0x7f9ba9160c90 = 0xffffffff81a00758 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[202] @0x7f9ba9160ca0 = 0xffffffff81a00760 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[203] @0x7f9ba9160cb0 = 0xffffffff81a00768 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[204] @0x7f9ba9160cc0 = 0xffffffff81a00770 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[205] @0x7f9ba9160cd0 = 0xffffffff81a00778 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[206] @0x7f9ba9160ce0 = 0xffffffff81a00780 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[207] @0x7f9ba9160cf0 = 0xffffffff81a00788 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[208] @0x7f9ba9160d00 = 0xffffffff81a00790 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[209] @0x7f9ba9160d10 = 0xffffffff81a00798 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[210] @0x7f9ba9160d20 = 0xffffffff81a007a0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[211] @0x7f9ba9160d30 = 0xffffffff81a007a8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[212] @0x7f9ba9160d40 = 0xffffffff81a007b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[213] @0x7f9ba9160d50 = 0xffffffff81a007b8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[214] @0x7f9ba9160d60 = 0xffffffff81a007c0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[215] @0x7f9ba9160d70 = 0xffffffff81a007c8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[216] @0x7f9ba9160d80 = 0xffffffff81a007d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[217] @0x7f9ba9160d90 = 0xffffffff81a007d8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[218] @0x7f9ba9160da0 = 0xffffffff81a007e0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[219] @0x7f9ba9160db0 = 0xffffffff81a007e8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[220] @0x7f9ba9160dc0 = 0xffffffff81a007f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[221] @0x7f9ba9160dd0 = 0xffffffff81a007f8 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[222] @0x7f9ba9160de0 = 0xffffffff81a00800 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[223] @0x7f9ba9160df0 = 0xffffffff81a00808 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[224] @0x7f9ba9160e00 = 0xffffffff81a00810 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[225] @0x7f9ba9160e10 = 0xffffffff81a00818 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[226] @0x7f9ba9160e20 = 0xffffffff81a00820 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[227] @0x7f9ba9160e30 = 0xffffffff81a00828 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[228] @0x7f9ba9160e40 = 0xffffffff81a00830 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[229] @0x7f9ba9160e50 = 0xffffffff81a00838 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[230] @0x7f9ba9160e60 = 0xffffffff81a00840 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[231] @0x7f9ba9160e70 = 0xffffffff81a00848 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[232] @0x7f9ba9160e80 = 0xffffffff81a00850 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[233] @0x7f9ba9160e90 = 0xffffffff81a00858 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[234] @0x7f9ba9160ea0 = 0xffffffff81a00860 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[235] @0x7f9ba9160eb0 = 0xffffffff81a00868 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[236] @0x7f9ba9160ec0 = 0xffffffff81a00870 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[237] @0x7f9ba9160ed0 = 0xffffffff81a00878 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[238] @0x7f9ba9160ee0 = 0xffffffff81a01f70 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[239] @0x7f9ba9160ef0 = 0xffffffff81a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[240] @0x7f9ba9160f00 = 0xffffffff81a00cf0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[241] @0x7f9ba9160f10 = 0xffffffff81a00c60 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[242] @0x7f9ba9160f20 = 0xffffffff81a00bd0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[243] @0x7f9ba9160f30 = 0xffffffff81a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[244] @0x7f9ba9160f40 = 0xffffffff81a02120 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[245] @0x7f9ba9160f50 = 0xffffffff81a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[246] @0x7f9ba9160f60 = 0xffffffff81a02510 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[247] @0x7f9ba9160f70 = 0xffffffff81a02000 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[248] @0x7f9ba9160f80 = 0xffffffff81a00b40 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[249] @0x7f9ba9160f90 = 0xffffffff81a02090 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[250] @0x7f9ba9160fa0 = 0xffffffff81a021b0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[251] @0x7f9ba9160fb0 = 0xffffffff81a02240 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[252] @0x7f9ba9160fc0 = 0xffffffff81a022d0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[253] @0x7f9ba9160fd0 = 0xffffffff81a02360 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[254] @0x7f9ba9160fe0 = 0xffffffff81a023f0 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] IDT[255] @0x7f9ba9160ff0 = 0xffffffff81a02480 (seg sel 0x10); p=1; dpl=0; type=14; ist=0
[idt.c] --------------------------------------------------------------------------------

--------------------------------------------------------------------------------
[main.c] Triggering user space software interrupts
--------------------------------------------------------------------------------


[main.c] ****** hello world from user space IRQ handler with count=0 ******
[file.c] reading buffer from '/dev/cpu/1/msr' (size=8)
[apic.c] established local memory mapping for APIC_BASE=0xfee00000 at 0x7f9ba915f000
[apic.c] APIC_ID=2000000; LVTT=400ee; TDCR=0
[main.c] APIC TPR/PPR is 0/0
[main.c] RSP at 0x7fff39d48a58
[main.c] RIP is 0x401297
[main.c] CS is 0x33
[main.c] EFLAGS is 0x206

[main.c] ****** hello world from user space IRQ handler with count=1 ******
[main.c] APIC TPR/PPR is 0/0
[main.c] RSP at 0x7fff39d48a58
[main.c] RIP is 0x401299
[main.c] CS is 0x33
[main.c] EFLAGS is 0x206

--------------------------------------------------------------------------------
[main.c] Triggering user space APIC timer interrupts
--------------------------------------------------------------------------------

[apic.c] APIC timer one-shot mode with division 2 (lvtt=2d/tdcr=0)

[main.c] ****** hello world from user space IRQ handler with count=2 ******
[main.c] APIC TPR/PPR is 0/32
[main.c] RSP at 0x7fff39d48a58
[main.c] RIP is 0x40130e
[main.c] CS is 0x33
[main.c] EFLAGS is 0x246
[main.c] returned from timer IRQ: flags=0x246

[main.c] ****** hello world from user space IRQ handler with count=3 ******
[main.c] APIC TPR/PPR is 0/32
[main.c] RSP at 0x7fff39d48a58
[main.c] RIP is 0x40130e
[main.c] CS is 0x33
[main.c] EFLAGS is 0x246
[main.c] returned from timer IRQ: flags=0x246

[main.c] ****** hello world from user space IRQ handler with count=4 ******
[main.c] APIC TPR/PPR is 0/32
[main.c] RSP at 0x7fff39d48a58
[main.c] RIP is 0x40130e
[main.c] CS is 0x33
[main.c] EFLAGS is 0x246
[main.c] returned from timer IRQ: flags=0x246
[apic.c] Restored APIC_LVTT=400ee/TDCR=0)
[file.c] writing buffer to '/dev/cpu/1/msr' (size=8)
[main.c] all is well; irq_handler_count=5; exiting..
```
