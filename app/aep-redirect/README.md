Sample output:

```
[main.c] Creating enclave...
==== Victim Enclave ====
[pt.c] /dev/sgx-step opened!
    Base:   0x7ff443400000
    Size:   4194304
    Limit:  0x7ff443800000
    TCS:    0x7ff44377a000
    SSA:    0x7ff44377bf48
    AEP:    0x402cb7
    EDBGRD: debug
[main.c] revoking a access rights..
[main.c] a at 0x7ff443619000
[pt.c] /dev/mem opened!
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | 0    | 0x001883172864 | x   | x | x   | 0 | 0 | x   | x   | 1   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | 0    | 0x547872636928 | x   | x | x   | 0 | 0 | x   | x   | 0   | 0   | 0 | 
+-------------------------------------------------------------------------------------------+
[main.c] calling enclave..
[main.c] Caught fault 11! Restoring access rights..
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | 0    | 0x001883172864 | x   | x | x   | 0 | 0 | x   | x   | 1   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
[main.c] Hello world from AEP callback with erip=0xc17! Resuming enclave..
=== SSA/GPRSGX region after AEX ===
    RAX:      0x7ff443400c1e
    RCX:      0x7ff443400c1e
    RDX:      0x1
    RBX:      0x0
    RSP:      0x7ff443769898
    RBP:      0x7ff443769cc0
    RSI:      0x0
    RDI:      0x0
    R8:       0x7ff4437fffff
    R9:       0x7ff443408476
    R10:      0x7ff443617888
    R11:      0x4
    R12:      0x0
    R13:      0x7ff443769c90
    R14:      0x0
    R15:      0x0
    RFLAGS:   0x10246
    RIP:      0x7ff443400c17
    URSP:     0x7fff8e7fbbe0
    URBP:     0x7fff8e7fc0a0
    EXITINFO: 0x0
    FSBASE:   0x7ff44378d000
    GSBASE:   0x7ff44378d000
[main.c] all is well; exiting..
```
