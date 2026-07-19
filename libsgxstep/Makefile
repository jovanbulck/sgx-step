-include Makefile.config
ZYDIS_DIR            = ../tools/disasm/zydis-amalgamated-v4.0.0

CC          = gcc
AS          = gcc
AR          = ar
ARFLAGS     = -rcs
LD          = gcc

ifeq ($(SHARED), 1)
    OUTPUT   = libsgx-step.so
    CFLAGS  += -DBUILD_SHARED -fPIC -fno-stack-protector -fno-builtin \
               -fno-common -Wno-attributes -g -D_GNU_SOURCE -O0 
    LDFLAGS += -shared  -L$(LIBSGXSTEP) -lsgx-step -lelf -pthread
    SOURCES  = $(shell ls *.c) $(ZYDIS_DIR)/Zydis.c
else
    OUTPUT   = libsgx-step.a
    SOURCES  = $(filter-out sgxstep-preload.c aep.c, $(shell ls *.c)) $(ZYDIS_DIR)/Zydis.c
endif

ifneq "$(WITH_PSTATE)" "0"
CPPFLAGS     += -DHAS_PSTATE
endif

ifneq "$(WITH_TURBO)" "0"
CPPFLAGS     += -DHAS_TURBO
endif

ifeq ($(M32), 1)
	ASFLAGS = -m32 -DM32=1
	CFLAGS += -m32 -DM32=1
endif

ASM          = $(shell ls *.S)

OBJECTS      = $(SOURCES:.c=.o) $(ASM:.S=.o)

CFLAGS              += -I$(ZYDIS_DIR)

ifeq ($(GRAMINE), 1)
	CFLAGS += -DSGX_SSAFRAMESIZE=16384
	SOURCES += ./../sdk/gramine/aep.c
endif

all: $(OUTPUT)

libsgx-step.so: $(OBJECTS) 
	echo "$(INDENT)[LD]" $(OBJECTS) -o $@
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@

libsgx-step.a: $(OBJECTS) 
	echo "$(INDENT)[AR]" -o $@
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

%.o : %.c
	echo "$(INDENT)[CC] " $<
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o : %.S
	echo "$(INDENT)[AS] " $<
	$(AS) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	echo "$(INDENT)[RM]" $(OBJECTS) $(OUTPUT)
	rm -f $(OBJECTS) $(OUTPUT)
