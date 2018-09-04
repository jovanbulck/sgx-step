#!/usr/bin/python3

import re
import os
from elftools.elf.elffile import ELFFile

IN_FILE          = 'out.txt'
ENCLAVE_FILE     = 'Enclave/encl.so'
STRLEN_SYM       = 'my_strlen'

# 64-bit
#    14c7:       48 89 f8                mov    %rdi,%rax
#    14ca:       80 38 00                cmpb   $0x0,(%rax)
#    14cd:       74 05                   je     14d4 <my_strlen+0xd>
#    14cf:       48 ff c0                inc    %rax
#    14d2:       eb f6                   jmp    14ca <my_strlen+0x3>
#    14d4:       48 29 f8                sub    %rdi,%rax
#    14d7:       c3                      retq   

# 32-bit
#    286a:       55                      push   %ebp
#    286b:       89 e5                   mov    %esp,%ebp
#    286d:       8b 55 08                mov    0x8(%ebp),%edx
#    2870:       89 d0                   mov    %edx,%eax
#    2872:       80 38 00                cmpb   $0x0,(%eax)
#    2875:       74 03                   je     287a <my_strlen+0x10>
#    2877:       40                      inc    %eax
#    2878:       eb f8                   jmp    2872 <my_strlen+0x8>
#    287a:       29 d0                   sub    %edx,%eax
#    287c:       5d                      pop    %ebp
#    287d:       c3                      ret

with open( ENCLAVE_FILE ,'rb') as f:
    elf = ELFFile(f)
    symtab = elf.get_section_by_name('.symtab')
    sym = symtab.get_symbol_by_name(STRLEN_SYM)
    strlen_addr = sym[0]['st_value']

if not os.environ.get("M32"):
    CMP             = strlen_addr+3
    JE              = CMP+3
    INC             = JE+2
    JMP             = INC+3
    SUB             = JMP+2
else:
    CMP             = strlen_addr+8
    JE              = CMP+3
    INC             = JE+2
    JMP             = INC+1
    SUB             = JMP+2

inst_stream = (CMP, JE, INC, JMP)

cur_inst = 0
prev_inst = 0

#for i in inst_stream:
#    print(hex(i))

print("parse_stlren.py: found strlen func at {} (length={})".format(hex(strlen_addr), len(inst_stream)))

count_tot  = 0
count_zero = 0
count_one  = 0
count_plus = 0
count_it   = 0
in_strlen  = 0

with open(IN_FILE, 'r') as fi:
    for line in fi:
        m = re.search('RIP=0x([0-9A-Fa-f]+); ACCESSED=([0-9])', line)
        if m:
            cur = int(m.groups()[0], base=16) 
            a = int(m.groups()[1]) 
            if (cur >= CMP and cur <= JMP):
                in_strlen = 1
                count_tot += 1
                expected = inst_stream[cur_inst]

                if (cur == prev_inst):
                    count_zero += 1
                    if a:
                        print("parse_strlen.py: WARNING: code PTE 'accessed' bit set for zero-step at", hex(cur))
                elif (cur == expected):
                    count_one += 1
                    if not a:
                        print("parse_strlen.py: WARNING: code PTE 'accessed' bit not set for single-step at", hex(cur))
                    prev_inst = inst_stream[cur_inst]
                    cur_inst = (cur_inst + 1) % len(inst_stream)
                elif (cur != expected):
                    skip = 0
                    while (cur != inst_stream[cur_inst]):
                        cur_inst = (cur_inst + 1) % len(inst_stream)
                        skip += 1
                    print("parse_strlen.py: skipped", skip)
                    count_plus += 1

            elif in_strlen:
                count_it += 1
                in_strlen = 0
                cur_inst = 0
                if (count_it == 10000):
                    break

print("parse_strlen.py counted it=", count_it, "tot=", count_tot, " one=", count_one, " zero=", count_zero, " plus=", count_plus)
