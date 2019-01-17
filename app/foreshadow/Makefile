LIBSGXSTEP_DIR       = ../..
LIBSGXSTEP           = $(LIBSGXSTEP_DIR)/libsgxstep
-include $(LIBSGXSTEP)/Makefile.config

ifeq ($(SGX_SDK),)
    SGX_SDK          = /opt/intel/sgxsdk
endif
export SGX_SDK
ifneq ($(SGX_SDK), /opt/intel/sgxsdk)
    URTS_LD_LIBRARY_PATH = LD_LIBRARY_PATH=$(LIBSGXSTEP_DIR)/linux-sgx/psw/urts/linux
endif

ENCLAVE              = Enclave
SUBDIRS              = $(ENCLAVE) $(LIBSGXSTEP)

CC                   = gcc
AS                   = gcc
LD                   = gcc

ifeq ($(M32), 1)
	ASFLAGS  = -m32 -DM32=$(M32)
	CFLAGS   = -m32 -DM32=$(M32)
	LDFLAGS  = -m32
else
	LIB_SUFX = 64
endif

CFLAGS              += -fPIC -fno-stack-protector -fno-builtin -fno-jump-tables \
                       -fno-common -Wno-attributes -g -D_GNU_SOURCE -O0
INCLUDE              = -I$(SGX_SDK)/include/ -I$(LIBSGXSTEP_DIR)
LDFLAGS             += -lsgx-step -lencl_proxy -lsgx_urts \
                       -lsgx_uae_service -pthread $(SUBDIRS:%=-L %) -L$(SGX_SDK)/lib$(LIB_SUFX)/ \
                       -L$(LIBSGXSTEP_DIR)/linux-sgx/psw/urts/linux

SOURCES              = $(shell ls *.c)
OBJECTS              = $(SOURCES:.c=.o)
OUTPUT               = app

BUILDDIRS            = $(SUBDIRS:%=build-%)
CLEANDIRS            = $(SUBDIRS:%=clean-%)

WATCH                = watch -n 0.1 #-e

.SILENT:
all: $(OUTPUT)
	
run: clean all
	sudo $(URTS_LD_LIBRARY_PATH) ./app 

repeat: clean all
	while true; do \
        $(WATCH) sudo $(URTS_LD_LIBRARY_PATH) ./app ; \
	done;

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
