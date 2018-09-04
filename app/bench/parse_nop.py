#!/usr/bin/python3

import re
import sys
from elftools.elf.elffile import ELFFile
    
if (len(sys.argv) != 2):
    print("usage: parse_nop.py expects one argument <inst_slide_len>")
    exit(1)

IN_FILE          = 'out.txt'
OUT_FILE         = 'parsed_nop.txt'
ENCLAVE_FILE     = 'Enclave/encl.so'
INST_SLIDE_SYM   = 'asm_microbenchmark'

with open( ENCLAVE_FILE ,'rb') as f:
    elf = ELFFile(f)
    symtab = elf.get_section_by_name('.symtab')
    sym = symtab.get_symbol_by_name(INST_SLIDE_SYM)
    inst_slide_start = sym[0]['st_value']

INST_SLIDE_START = inst_slide_start
INST_SLIDE_LEN   = int(sys.argv[1])
INST_SLIDE_END   = INST_SLIDE_START + INST_SLIDE_LEN
print("parse_nop.py: found instruction slide at {} (length={})".format(hex(INST_SLIDE_START), INST_SLIDE_LEN))

count_tot  = 0
count_zero = 0
count_one  = 0
count_plus = 0
prev       = 0

with open(IN_FILE, 'r') as fi, open(OUT_FILE, 'w') as fo:
    for line in fi:
        m = re.search('RIP=0x([0-9A-Fa-f]+); ACCESSED=([0-9])', line)
        if m:
            cur = int(m.groups()[0], base=16) 
            a = int(m.groups()[1]) 
            if (cur >= INST_SLIDE_START) and (cur <= INST_SLIDE_END):
                diff = cur - prev
                if prev:
                    fo.write(str(diff) + '\n')
                if diff == 0:
                    count_zero += 1
                    if a:
                        print("parse_nop.py: WARNING: code PTE 'accessed' bit set for zero-step at", hex(cur))
                elif diff == 1:
                    count_one += 1
                    if not a:
                        print("parse_nop.py: WARNING: code PTE 'accessed' bit not set for single-step at", hex(cur))
                elif prev:
                    print("parse_nop.py: WARNING: non-single step diff:", diff, "prev=", hex(prev), " cur=", hex(cur))
                    count_plus += 1
                prev = cur
                count_tot += 1

print("parse_nop.py counted tot=", count_tot, " one=", count_one, " zero=", count_zero, " plus=", count_plus)
