#!/usr/bin/python3
import re

IN_FILE          = 'out.txt'
OUT_FILE         = 'parsed_zz.txt'

# XXX fill in appropriate address from objdump encl.so here..
BLOCK0          = 0x1488
BLOCK0_J        = BLOCK0+25
BLOCK1          = BLOCK0_J+2
BLOCK1_J        = BLOCK1+8
BLOCK2          = BLOCK1_J+2
BLOCK2_J        = BLOCK2+25
BLOCK3          = BLOCK2_J+2
BLOCK3_J        = BLOCK3+8
BLOCK4          = BLOCK3_J+2
BLOCK5          = BLOCK4+1
ZZ1             = BLOCK5+10
ZZ2             = ZZ1+2
ZZ3             = ZZ2+2
ZZ4             = ZZ3+2


inst_stream = (BLOCK0, BLOCK0+7, BLOCK0+14, BLOCK0+21, BLOCK0_J, ZZ1, BLOCK1_J,
               ZZ2, BLOCK2_J, ZZ3, BLOCK3_J, ZZ4, BLOCK1, BLOCK1+1, BLOCK1_J,
               ZZ2, BLOCK2_J, ZZ3, BLOCK3_J, ZZ4, BLOCK5)

cur_inst = 0
prev_inst = 0

#for i in inst_stream:
#    print(hex(i))

print("parse_zz.py: inst stream has length=", len(inst_stream))

count_tot  = 0
count_zero = 0
count_one  = 0
count_plus = 0
count_it   = 0
in_zz      = 0

with open(IN_FILE, 'r') as fi, open(OUT_FILE, 'w') as fo:
    for line in fi:
        m = re.search('offset=0x([0-9A-Fa-f]+)', line)
        if m:
            cur = int(m.groups()[0], base=16) 
            if (cur >= BLOCK0 and cur <= BLOCK5) or (cur >= ZZ1 and cur <= ZZ4):
                in_zz = 1
                count_tot += 1
                expected = inst_stream[cur_inst]
                if (cur == prev_inst):
                    count_zero += 1
                elif (cur == expected):
                    count_one += 1
                    prev_inst = inst_stream[cur_inst]
                    cur_inst = (cur_inst + 1) % len(inst_stream)
                elif (cur != expected):
                    skip = 0
                    while (cur != inst_stream[cur_inst]):
                        cur_inst = (cur_inst + 1) % len(inst_stream)
                        skip += 1
                    print("parse_zz.py: skipped", skip)
                    count_plus += 1

            elif in_zz:
                count_it += 1
                in_zz = 0
                if (count_it == 10000):
                    break

print("parse_zz.py counted it=", count_it, "tot=", count_tot, " one=", count_one, " zero=", count_zero, " plus=", count_plus)
