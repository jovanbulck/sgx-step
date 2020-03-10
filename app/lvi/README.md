## LVI: Hijacking transient execution through microarchitectural load value injection

## Building and running

First make sure you have a working SGX-Step installation, as explained in the
top-level [README](../../README.md).  Note that the LVI PoCs in this directory
only use `libsgxstep`'s page-table manipulation features, so setting up
single-stepping is not required.

## LVI-SB: Store buffer injection

Consider the minimal example gadget code below:

--------------------------------------------------------------------------------
```C
1 void ecall_lvi_store_enclave(char *user_pt, char *oracle)
2 {
3   *user_pt = 'S';                  /* VICTIM STORE: write dummy value to extra-enclave user memory */
4   trusted_val = *enclave_pt;       /* VICTIM LOAD: inject 'S' and replace trusted enclave load value via store buffer 4K alias */
5   leak = oracle[4096*trusted_val]; /* VICTIM ENCODE: e.g., cache-based covert channel gadget */
6 }
```
--------------------------------------------------------------------------------

**Attack procedure.** Importantly, this attack proceeds entirely _within_ the
victim enclave. We abuse that SGX enclaves live inside a conventional
surrounding host virtual address space and frequently dereference _untrusted_
"user" pointers to copy to/from memory outside the enclave. While such pointers
are always properly verified to lie outside the enclave, the attacker still
controls the 4K offset in untrusted memory.

Hence, the key idea is to provoke targeted illegal store-to-load forwarding
_inside_ the enclave code by forcing a 4K alias collision between the trusted
in-enclave load pointer at line (4) and an earlier user-provided out-enclave
store at line (3). Since page tables are under control of an SGX attacker, the
load at line (4) can be made to fault and illegal store-to-load forwarding
subsequently causes the transient instructions at line (5) to compute on the
illegal attacker value stored at line (3) instead of the enclave data as
intended by the programmer.

Much like in a Spectrev1 attack, the adversary now relies on the presence of
"confused deputy" gadget code inside the victim to transmit the secret (e.g.,
through a cache-based covert channel as demonstrated in line 5). Finally, after
the fault at line (4) is raised and the transient instruction results are
discarded, the attacker reconstructs the tranmission from the
microarchitectural state in the exception handler before restoring page access
rights and resuming the enclave.

Note that this proof-of-concept gadget only serves to demonstrate that enclaves
can be made to transiently compute on wrong data, but this specific gadget
itself is harmless. It only encodes the injected dummy data 'S'. However, as
demonstrated in the paper, real-world LVI adversaries can mount more advanced
attacks to arbitrarily hijack the enclave's control and/or data flow and encode
arbitrary secrets.

**Suggested mitigation.** Insert an LFENCE instruction to halt the processor pipeline after the potentially faulting trusted enclave load operation (in between lines 4 and 5).

**Proof-of-concept code.** Enable this PoC through `#define LVI_SB 1` in `main.c`. Example output for `make run` as follows (on a processor vulnerable to [Fallout](https://mdsattacks.com/)/[MSBDS](https://software.intel.com/security-software-guidance/insights/deep-dive-intel-analysis-microarchitectural-data-sampling)):

```$ make run
Flush+Reload Threshold: 224
[sched.c] continuing on CPU 1
[main.c] Creating enclave...
==== Victim Enclave ====
[pt.c] /dev/sgx-step opened!
    Base:   0x7fea68000000
    Size:   8388608
    Limit:  0x7fea68800000
    TCS:    0x7fea684c4000
    SSA:    0x7fea684c5f48
    AEP:    0x7fea6a973845
    EDBGRD: debug
[main.c] oracle at 0x5590313d7000
[main.c] user_page at 0x5590314d7000
[main.c] enclave_page_a at 0x7fea68220000 w PTE
[pt.c] /dev/mem opened!
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | 0    | 0x000070675000 | x   | x | x   | 1 | 1 | x   | x   | 1   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
[main.c] enclave_page_b at 0x7fea6821f000 w PTE
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | 0    | 0x000070676000 | x   | x | x   | 1 | 1 | x   | x   | 1   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S S 
```

## LVI-SB: L1D injection

Consider the minimal example gadget code below:

--------------------------------------------------------------------------------
```C
1 char __attribute__((aligned(0x1000))) page_a[4096] = {'A'}, page_b[4096] = {'B'};
2 
3 void ecall_lvi_remap(char *oracle)
4 {
5   a = *page_a;           /* VICTIM PREFETCH: load 'A' at valid enclave physical address into L1 */
7   b = *page_b;           /* VICTIM LOAD: inject 'A' from remapped physical address and override trusted value 'B' */
8   leak = oracle[4096*b]; /* VICTIM ENCODE: e.g., cache-based covert channel gadget */
9 }
```
--------------------------------------------------------------------------------

**Attack procedure.** The above dummy enclave contains two valid physical enclave
pages containing either data 'A' or 'B'. In line with the enclave programmer's
intentions, Intel SGX's EPCM checks dictate that these pages should be accessed through
respectively virtual addresses `page_a` and `page_b`. We first consider the
case of a normal enclave execution that does not suffer any page faults. At
line (5), the virtual address `page_a` is dereferenced and the physical memory
containing 'A' is brought into the L1D cache. Note that in our experimental
setup, we ensure that any pending load/store buffer are drained and 'A' is
indeed committed in the L1D cache by explicitly including an MFENCE at line
(6).  Next, at line (7), the enclave dereferences virtual address `page_b` and
loads the physical memory containing 'B' into L1D. Architectural program
semantics imply that the enclave should only ever compute on the value 'B'
after line (7). Our minimalistic experiment aims to verify this by encoding the
value read at virtual address `page_b` over an explicit cache-based covert
channel on line (8).

We now consider the case of an LVI attacker. Before entering the victim
enclave, the root adversary remaps the PTE for `page_b` to point to the
physical page frame number of `page_a`. Since the virtual-to-physical mapping
for `page_a` remains valid and unchanged, line (5) still successfully loads the
physical memory containing 'A' into the L1D cache. Next, at line (7), the CPU
dereferences the virtual address `page_b` and walks the page tables to
establish the corresponding physical address (i.e., the "poisoned" page frame
number set up by the attacker). To maintain architectural consistency, SGX's
additional EPCM checks will eventually detect the malicious virtual-to-physical
address remapping attack attempt and raise a page fault at line (7). Crucially,
however, before the fault is architecturally raised, the poisoned physical
address is still sent to the L1D cache. Since the physical memory location
holding 'A' was brought into the L1D cache earlier during the victim enclave's
own valid execution at line (5), a cache hit occurs and 'A' is injected into
the transient execution at line (8).

Note that for clarity we focussed on an artificial cache-based covert channel
gadget in the example above, but we essentially demonstrated that _arbitrary_
enclave loads can be transiently replaced with _any_ value residing in L1D at
the same 4K offset. Hence, analogous to the store buffer injection techniques
above, adversaries may for instance arbitrarily hijack indirect control flow in
the transient domain. Importantly, however, L1 injection allows such control
flow hijacking even when no prior stores are controlled by the attacker or when
the store buffer is explicitly drained (MFENCE). Likewise, the above code
example demonstrated address remapping between two in-enclave pages, but we
also successfully demonstrated transiently remapping an enclave page to a
user-controlled page outside the enclave.

**Suggested mitigation.** Insert an LFENCE instruction to halt the processor
pipeline after the potentially faulting trusted enclave load operation (in
between lines 7 and 8).

**Proof-of-concept code.** Enable this PoC through `#define LVI_L1D 1` in `main.c`. Example output for `make run` as follows (on a processor vulnerable to [Foreshadow](https://foreshadowattack.eu/)/[L1TF](https://software.intel.com/security-software-guidance/insights/deep-dive-intel-analysis-l1-terminal-fault)):

```$ make run
Flush+Reload Threshold: 226
[sched.c] continuing on CPU 1
[main.c] Creating enclave...
==== Victim Enclave ====
[pt.c] /dev/sgx-step opened!
    Base:   0x7fbdd4800000
    Size:   8388608
    Limit:  0x7fbdd5000000
    TCS:    0x7fbdd4cc4000
    SSA:    0x7fbdd4cc5f48
    AEP:    0x7fbdd6fd4845
    EDBGRD: debug
[main.c] oracle at 0x562f1127b000
[main.c] user_page at 0x562f1137b000
[main.c] enclave_page_a at 0x7fbdd4a20000 w PTE
[pt.c] /dev/mem opened!
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | 0    | 0x000070676000 | x   | x | x   | 1 | 1 | x   | x   | 1   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
[main.c] enclave_page_b at 0x7fbdd4a1f000 w PTE
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | 0    | 0x000070677000 | x   | x | x   | 1 | 1 | x   | x   | 1   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A A 
```

## LVI-SB-ROP: Transient control flow hijacking

Consider the minimal example code gadget below:

--------------------------------------------------------------------------------
```C
1 void ecall_lvi_store_user(uint64_t *user_pt, uint64_t user_val, char *oracle)
2 {
3   *user_pt = user_val;    /* VICTIM STORE: write user-controlled value to extra-enclave user memory */
4   return;                 /* VICTIM LOAD: inject from user store 4K alias (i.e., control flow hijack) */
6 }
```
--------------------------------------------------------------------------------

**Attack procedure.** We again rely on an attacker-controlled user pointer
dereference inside the enclave. Where the above LVI variant faulted on user
memory loads, we now provoke page faults on legitimate enclave memory load
instructions.  Specifically we require that the enclave first stores a
(user-controlled) value to a user-controlled untrusted address (line 3).
Subsequently, the enclave "return" statement performs a _legit_ load from the
enclave stack (line 4) and uses that value as a trusted return address to
redirect control flow.

Architecturally the enclave will always follow the intended control/data flow,
but an SGX page table adversary may force the legit load on line (4) to fault
and read the untrusted attacker value from the store buffer instead of from
trusted enclave memory. The transient instructions following the faulting load
will now compute on the value from the store buffer and hence redirect
transient control flow to an arbitrary attacker-controlled location. Much like
Spectre v2 branch poisoning attacks, this ultimately allows attackers to abuse
"2nd-stage gadgets" in the existing enclave code base to dereference arbitrary
secrets and encode them in the microarchitectural CPU state.

Note that for clarity we focused on hijacking RET control flow in the above
example, but we also demonstrated successful LVI attacks for JMP/CALL indirect
control flow instructions. This effectively shows that we can arbitrarily
hijack transient return control flow, even in the presence of strong
Spectre-RSB/BTB mitigations that flush shared RSB/BTB buffers on enclave entry.
Interestingly, we also found that large or multiple user stores enable
attackers to setup a fake "transient stack" in the store buffer so as to
repeatedly inject illegal values for consecutive enclave stack loads (POP/RET
sequences). Much like in architectural ROP, such capability allows to chain
together multiple 2nd stage gadgets to compose arbitrary transient computations
of interest.

**Suggested mitigation.** Insert LFENCE instructions after loading branch target addresses from trusted memory, and before redirecting control flow. Break up x86 `ret` instructions into explicit load and branch instructions.

**Proof-of-concept code.** Enable this PoC through `#define LVI_SB_ROP 1` in `main.c`. Example output for `make run` as follows (on a processor vulnerable to [Fallout](https://mdsattacks.com/)/[MSBDS](https://software.intel.com/security-software-guidance/insights/deep-dive-intel-analysis-microarchitectural-data-sampling)):

``` $ make run
Flush+Reload Threshold: 227
[sched.c] continuing on CPU 1
[main.c] Creating enclave...
==== Victim Enclave ====
[pt.c] /dev/sgx-step opened!
    Base:   0x7f8e67000000
    Size:   8388608
    Limit:  0x7f8e67800000
    TCS:    0x7f8e67481000
    SSA:    0x7f8e67482f48
    AEP:    0x7f8e695fa845
    EDBGRD: debug
[main.c] oracle at 0x563fd9561000
[main.c] user_page at 0x563fd9661000
[main.c] enclave_page_a at 0x7f8e6721e000 w PTE
[pt.c] /dev/mem opened!
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | 0    | 0x00007053e000 | x   | x | x   | 1 | 1 | x   | x   | 1   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
[main.c] enclave_page_b at 0x7f8e6721d000 w PTE
+-------------------------------------------------------------------------------------------+
| XD | PK | IGN | RSVD | PHYS ADRS      | IGN | G | PAT | D | A | PCD | PWT | U/S | R/W | P | 
| 0  | x  | x   | 0    | 0x00007053f000 | x   | x | x   | 1 | 1 | x   | x   | 1   | 1   | 1 | 
+-------------------------------------------------------------------------------------------+
R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R R
```
