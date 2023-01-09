# Test for the kernel mapped interupt handlers

This test uses the kernel mapped ISR region to demonstrate sw IRQ handling from other processes and cores.

## Usage:
Start the listener:
```
sudo ./app   
```

Trigger the SW IRQ from another process:
```
taskset -c 1 ./app trigger
```


# Sample output:


```
[idt.c] locking IRQ handler pages 0x564b69f22000/0x564b69f21000
[idt.c] setting up isr mapping: from 0x564b69f21000 to 0x564b69f2205b
[pt.c] /dev/sgx-step opened!
[idt.c] we received the base address from kernel 0xffffc900201ea000
[idt.c] the offset to the kernel mapped ISR region is 0xffff72b4b62c9000
[pt.c] /dev/mem opened!
[sched.c] continuing on CPU 1
[idt.c] DTR.base=0xfffffe0000000000/size=4095 (256 entries)
[idt.c] established user space IDT mapping at 0x7f4302014000

--------------------------------------------------------------------------------
[main.c] Installing and testing ring0 IDT handler
--------------------------------------------------------------------------------

[idt.c] using kernel mapped ISR handler: 0x564b69f22000 -> 0xffffc900201eb000
[idt.c] installed asm IRQ handler at 10:0x564b69f22000
[idt.c] IDT[ 45] @0x7f43020142d0 = 0xffffc900201eb000 (seg sel 0x10); p=1; dpl=3; type=14; ist=0
[main.c] wating for sw IRQ on vector 45 ...
[main.c] returned from IRQ: my_cpl=3; irq_cpl=0; count=01; flags=0x246; nemesis=835151372
[main.c] all is well; irq_count=1; exiting..
```
