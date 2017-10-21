LIBSGXSTEP_DIR       = ../..
LIBSGXSTEP           = $(LIBSGXSTEP_DIR)/libsgxstep
ENCLAVE              = Enclave
SUBDIRS              = $(ENCLAVE) $(LIBSGXSTEP)

CC                   = gcc
AS                   = gcc
LD                   = gcc

CFLAGS              += -fPIC -fno-stack-protector -fno-builtin -fno-jump-tables \
                       -fno-common -Wno-attributes -g -D_GNU_SOURCE -O0
INCLUDE              = -I/opt/intel/sgxsdk/include/ -I$(LIBSGXSTEP_DIR)
LDFLAGS             += -lsgx-step -lencl_proxy -lsgx_urts \
                       -lsgx_uae_service $(SUBDIRS:%=-L %) 

SOURCES              = $(shell ls *.c)
OBJECTS              = $(SOURCES:.c=.o)
OUTPUT               = app

BUILDDIRS            = $(SUBDIRS:%=build-%)
CLEANDIRS            = $(SUBDIRS:%=clean-%)

ATTACK = 1
PARSE  = nop
ifeq ($(STRLEN), 1)
    ATTACK = 2
    PARSE  = strlen
endif
ifeq ($(ZIGZAG), 1)
    ATTACK = 3
    PARSE  = zz
endif

ifeq ($(NUM),)
    NUM             = 100
endif
export NUM

CFLAGS              += -DATTACK_SCENARIO=$(ATTACK) -DNUM_RUNS=$(NUM)

.SILENT:
all: $(OUTPUT)
	
run: clean all
	sudo ./app > out.txt

parse: run
	./parse_$(PARSE).py $(NUM)

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
