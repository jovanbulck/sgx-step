CC          = gcc
AR          = ar
ARFLAGS     = -rcs

CFLAGS     += -DHAS_PSTATE -DHAS_TURBO

INCLUDE     = -I/opt/intel/sgxsdk/include/
SOURCES     = $(shell ls *.c)
OBJECTS     = $(SOURCES:.c=.o) aep_trampoline.o
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
	$(AS) $(INCLUDE) -c $< -o $@

clean:
	echo "$(INDENT)[RM]" $(OBJECTS) $(OUTPUT)
	rm -f $(OBJECTS) $(OUTPUT)
