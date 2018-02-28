#!/usr/bin/python3
import re
import sys

if (len(sys.argv) != 2):
    print("usage: parse_nop.py expects one argument <inst_slide_len>")
    exit(1)

IN_FILE          = 'out.txt'
OUT_FILE         = 'parsed_nop.txt'

# XXX fill in appropriate address from objdump encl.so here..
INST_SLIDE_START = 0x1523
INST_SLIDE_LEN   = int(sys.argv[1])
INST_SLIDE_END   = INST_SLIDE_START + INST_SLIDE_LEN

count_tot  = 0
count_zero = 0
count_one  = 0
count_plus = 0
prev       = 0

with open(IN_FILE, 'r') as fi, open(OUT_FILE, 'w') as fo:
    for line in fi:
        m = re.search('offset=0x([0-9A-Fa-f]+)', line)
        if m:
            cur = int(m.groups()[0], base=16) 
            if (cur >= INST_SLIDE_START) and (cur <= INST_SLIDE_END):
                diff = cur - prev
                if prev:
                    fo.write(str(diff) + '\n')
                if diff == 0:
                    count_zero += 1
                elif diff == 1:
                    count_one += 1
                elif prev:
                    print("parse_nop.py: non-single step diff:", diff, "prev=", prev, " cur=", cur)
                    count_plus += 1
                prev = cur
                count_tot += 1

print("parse_nop.py counted tot=", count_tot, " one=", count_one, " zero=", count_zero, " plus=", count_plus)
