#!/usr/bin/python3
import re
import os
from elftools.elf.elffile import ELFFile

IN_FILE          = 'out.txt'
ENCLAVE_FILE     = 'Enclave/encl.so'

def get_sym_addr(name, symtab):
    return symtab.get_symbol_by_name(name)[0]['st_value']

with open( ENCLAVE_FILE ,'rb') as f:
    elf = ELFFile(f)
    symtab = elf.get_section_by_name('.symtab')
    BLOCK0          = get_sym_addr("block0"  , symtab)
    BLOCK0_J        = get_sym_addr("block0.j", symtab)
    BLOCK1          = get_sym_addr("block1"  , symtab)
    BLOCK1_J        = get_sym_addr("block1.j", symtab)
    BLOCK2          = get_sym_addr("block2"  , symtab)
    BLOCK2_J        = get_sym_addr("block2.j", symtab)
    BLOCK3          = get_sym_addr("block3"  , symtab)
    BLOCK3_J        = get_sym_addr("block3.j", symtab)
    BLOCK4          = get_sym_addr("block4"  , symtab)
    BLOCK5          = get_sym_addr("block5"  , symtab)
    ZZ1             = get_sym_addr("zz1"     , symtab)
    ZZ2             = get_sym_addr("zz2"     , symtab)
    ZZ3             = get_sym_addr("zz3"     , symtab)
    ZZ4             = get_sym_addr("zz4"     , symtab)

if not os.environ.get("M32"):
    inst_stream = (BLOCK0, BLOCK0+7, BLOCK0+14, BLOCK0+21, BLOCK0_J, ZZ1, BLOCK1_J,
               ZZ2, BLOCK2_J, ZZ3, BLOCK3_J, ZZ4, BLOCK1, BLOCK1+1, BLOCK1_J,
               ZZ2, BLOCK2_J, ZZ3, BLOCK3_J, ZZ4, BLOCK5)
else:
    inst_stream = (BLOCK0, BLOCK0+5, BLOCK0+6, BLOCK0+12, BLOCK0+17, BLOCK0+18,
                BLOCK0+24, BLOCK0+29, BLOCK0+30, BLOCK0+36, BLOCK0+39, BLOCK0_J,
                ZZ1, BLOCK1_J, ZZ2, BLOCK2_J, ZZ3, BLOCK3_J, ZZ4, BLOCK1, BLOCK1+1,
                BLOCK1+6, BLOCK1+7, BLOCK1_J, ZZ2, BLOCK2_J, ZZ3, BLOCK3_J, ZZ4, BLOCK5)

cur_inst = 0
prev_inst = 0

#for i in inst_stream:
#    print(hex(i))

print("parse_zz.py: found Zigzagger block0 at {} (length={})".format(hex(BLOCK0), len(inst_stream)))

count_tot  = 0
count_zero = 0
count_one  = 0
count_plus = 0
count_it   = 0
in_zz      = 0

with open(IN_FILE, 'r') as fi:
    for line in fi:
        m = re.search('RIP=0x([0-9A-Fa-f]+); ACCESSED=([0-9])', line)
        if m:
            cur = int(m.groups()[0], base=16) 
            a = int(m.groups()[1]) 
            if (cur >= BLOCK0 and cur <= BLOCK5) or (cur >= ZZ1 and cur <= ZZ4):
                in_zz = 1
                count_tot += 1
                expected = inst_stream[cur_inst]
                if (cur == prev_inst):
                    count_zero += 1
                    if a:
                        print("parse_zz.py: WARNING: code PTE 'accessed' bit set for zero-step at", hex(cur))
                elif (cur == expected):
                    count_one += 1
                    if not a:
                        print("parse_zz.py: WARNING: code PTE 'accessed' bit not set for single-step at", hex(cur))
                    prev_inst = inst_stream[cur_inst]
                    cur_inst = (cur_inst + 1) % len(inst_stream)
                elif (cur != expected):
                    skip = 0
                    while (cur != inst_stream[cur_inst]):
                        cur_inst = (cur_inst + 1) % len(inst_stream)
                        skip += 1
                    print("parse_zz.py: WARNING skipped", skip)
                    count_plus += 1

            elif in_zz:
                count_it += 1
                in_zz = 0
                if (count_it == 10000):
                    break

print("parse_zz.py counted it=", count_it, "tot=", count_tot, " one=", count_one, " zero=", count_zero, " plus=", count_plus)
