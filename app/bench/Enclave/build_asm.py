#!/usr/bin/python3

import string
import sys

if (len(sys.argv) != 2):
    print("usage: build_asm.py expects one argument <inst_slide_len>")
    exit(1)

NB_INST     = int(sys.argv[1])
ASM_INST    = "nop"

template = string.Template('''
    /* ====== auto generated asm code from Python script ======= */

    .text
    .global asm_microbenchmark
    .align 0x1000 /* 4KiB */
    asm_microbenchmark:
    $asmCode

micro_done:
    ret
''')

asm = (ASM_INST + '\n') * NB_INST
code = template.substitute(asmCode=asm)

with open('asm_nop.S', 'w') as the_file:
    the_file.write(code)
