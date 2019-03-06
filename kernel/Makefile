LKM             = sgx-step
SGX_DRIVER_DIR  = ../

ifndef NO_SGX
   export NO_SGX=0
endif

obj-m          += $(LKM).o
$(LKM)-objs     = sgxstep.o
ccflags-y      := -I$(src)/$(SGX_DRIVER_DIR) -DNO_SGX=$(NO_SGX)

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
