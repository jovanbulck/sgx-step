LKM             = sgx-step
PWD            := $(shell pwd)

obj-m          += $(LKM).o
$(LKM)-objs     = sgxstep.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

unload:
	sudo rmmod $(LKM).ko || true

load: unload all
	sudo modprobe -a isgx msr || true
	sudo insmod $(LKM).ko
	sudo dmesg | tail
