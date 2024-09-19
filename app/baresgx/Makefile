LIBSGXSTEP_DIR          = ../..
LIBSGXSTEP              = $(LIBSGXSTEP_DIR)/libsgxstep
-include $(LIBSGXSTEP)/Makefile.config
LIBSGXSTEP_SILENT       = 1

BARESGX                 = ../../sdk/bare-sgx/urts
ENCLAVE                 = enclave


SUBDIRS                 = $(ENCLAVE) $(BARESGX) $(LIBSGXSTEP)

CC                      := gcc
LD                      := gcc
INCLUDE                 := -I$(BARESGX)/include/ -I$(LIBSGXSTEP_DIR) 
CFLAGS                  := -Wall -g -fPIC 
LDFLAGS                 := -z noexecstack -pthread -lbaresgx-urts -lsgx-step -L$(BARESGX) -L$(LIBSGXSTEP) -lcrypto -lelf

SOURCES                 = $(shell ls *.c)
OBJECTS                 = $(SOURCES:.c=.o)
OUTPUT                  = app

BUILDDIRS               = $(SUBDIRS:%=build-%)
CLEANDIRS               = $(SUBDIRS:%=clean-%)

MAKEFLAGS               = --silent

all: $(OUTPUT)

$(OUTPUT): $(BUILDDIRS) $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT) 

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

$(CLEANDIRS):
	$(MAKE) clean -C $(@:clean-%=%)

clean: $(CLEANDIRS)
	rm -f $(OBJECTS) $(OUTPUT)
