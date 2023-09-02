.text
.global incs
incs:
    xor %rax, %rax
    .rept 1000000
       inc %rax 
    .endr
    ret

.data
.align 0x1000
.global my_page
my_page:
.word 0xdead
.space 0x1000
