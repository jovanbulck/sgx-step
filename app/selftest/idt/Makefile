LIBSGXSTEP_DIR       = ../../..
LIBSGXSTEP           = $(LIBSGXSTEP_DIR)/libsgxstep
NO_SGX               = 1
export NO_SGX
-include $(LIBSGXSTEP)/Makefile.config


SUBDIRS              = $(LIBSGXSTEP)

CC                   = gcc
AS                   = gcc
LD                   = gcc

CFLAGS              += -fPIC -fno-stack-protector -fno-builtin -fno-jump-tables \
                       -fno-common -Wno-attributes -g -D_GNU_SOURCE -O0
INCLUDE              = -I$(LIBSGXSTEP_DIR) 
LDFLAGS             += -lsgx-step -pthread $(SUBDIRS:%=-L %)

SOURCES              = $(shell ls *.c)
OBJECTS              = $(SOURCES:.c=.o)
OUTPUT               = app

BUILDDIRS            = $(SUBDIRS:%=build-%)
CLEANDIRS            = $(SUBDIRS:%=clean-%)

MAKEFLAGS           += --silent

all: $(OUTPUT)
	
run: clean all
	#TODO disable SMEP/SMAP at runtime through /dev/sgx-step
	!(cat /proc/cpuinfo | grep -q smep) || (echo "smep not disabled!" && false)
	!(cat /proc/cpuinfo | grep -q smap) || (echo "smap not disabled!" && false)
	sudo ./app

$(OUTPUT): $(BUILDDIRS) $(OBJECTS)
	echo "$(INDENT)[LD]" $(OBJECTS) $(LIBS) -o $(OUTPUT) 
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT) 

%.o : %.c
	echo "$(INDENT)[CC] " $<
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

%.o : %.S
	echo "$(INDENT)[AS] " $<
	$(AS) $(INCLUDE) -c $< -o $@

clean: $(CLEANDIRS)
	echo "$(INDENT)[RM]" $(OBJECTS) $(OUTPUT)
	rm -f $(OBJECTS) $(OUTPUT)

$(BUILDDIRS):
	echo "$(INDENT)[===] $(@:build-%=%) [===]"
	$(MAKE) -C $(@:build-%=%) INDENT+="$(INDENT_STEP)" curr-dir=$(curr-dir)/$(@:build-%=%)

$(CLEANDIRS):
	echo "$(INDENT)[===] $(@:clean-%=%) [===]"
	$(MAKE) clean -C $(@:clean-%=%) INDENT+="$(INDENT_STEP)" curr-dir=$(curr-dir)/$(@:build-%=%)
