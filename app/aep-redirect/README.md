Sample output:

```
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[main.c] Creating enclave...
==== Victim Enclave ====
    Base: 0x7f3d01c00000
    Size: 4194304
    Limit:  0x7f3d02000000
    TCS:  0x7f3d01f7a000
    AEP:  0x402926
    EDBGRD: debug
[main.c] revoking a access rights..
[main.c] a at 0x7f3d01e19000
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | x    | 0x002151641088 | x   | x | x   | 0 | 0 | x   | x   | x   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | x    | 0x547604168704 | x   | x | x   | 0 | 0 | x   | x   | x   | 0   | 0 | 
+-------------------------------------------------------------------------------------------+
[main.c] calling enclave..
[main.c] Caught fault 11! Restoring access rights..
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | x    | 0x002151641088 | x   | x | x   | 0 | 0 | x   | x   | x   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
[main.c] Hello world from AEP callback with erip=0x13d7! Resuming enclave..
[main.c] all is well; exiting..
```
