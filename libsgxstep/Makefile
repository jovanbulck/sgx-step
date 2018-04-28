CC          = gcc
AS          = gcc
AR          = ar
ARFLAGS     = -rcs
CFLAGS     += -DHAS_PSTATE -DHAS_TURBO

ifeq ($(M32), 1)
	ASFLAGS = -m32 -DM32=$(32BIT)
	CFLAGS += -m32
endif

INCLUDE     = -I$(SGX_SDK)/include/
SOURCES     = $(shell ls *.c)
ASM         = $(shell ls *.S)
OBJECTS     = $(SOURCES:.c=.o) $(ASM:.S=.o)
OUTPUT      = libsgx-step.a

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	echo "$(INDENT)[AR] " $(OUTPUT)
	$(AR) $(ARFLAGS) $(OUTPUT) $(OBJECTS)

%.o : %.c
	echo "$(INDENT)[CC] " $<
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

%.o : %.S
	echo "$(INDENT)[AS] " $<
	$(AS) $(ASFLAGS) $(INCLUDE) -c $< -o $@

clean:
	echo "$(INDENT)[RM]" $(OBJECTS) $(OUTPUT)
	rm -f $(OBJECTS) $(OUTPUT)
