LKM             = sgx-step
SGX_DRIVER_DIR  = ../

obj-m          += $(LKM).o
$(LKM)-objs     = sgxstep.o
ccflags-y      := -I$(src)/$(SGX_DRIVER_DIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

unload:
	sudo rmmod $(LKM).ko || true

load: unload all
	sudo modprobe -a isgx msr
	sudo insmod $(LKM).ko
	dmesg | tail
