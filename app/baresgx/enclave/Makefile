.PHONY: all clean

CFLAGS  += -Wall -Werror -static-pie -nostdlib -ffreestanding -fPIE \
	       -fno-stack-protector -mrdrnd $(INCLUDES)
LDFLAGS := -Wl,-T,encl.lds,--build-id=none

encl.elf: encl.S
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.elf *.o
