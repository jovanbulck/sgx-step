# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

include ../config.mk

LIBSGXSTEP_DIR=../../../
LIBSGXSTEP=$(LIBSGXSTEP_DIR)/libsgxstep

CFLAGS=$(shell pkg-config oehost-$(COMPILER) --cflags) -I$(LIBSGXSTEP_DIR) 
LDFLAGS=$(shell pkg-config oehost-$(COMPILER) --libs) -lsgx-step -pthread -L$(LIBSGXSTEP) -lelf
INCDIR=$(shell pkg-config oehost-$(COMPILER) --variable=includedir)

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../helloworld.edl --untrusted \
		--search-path $(INCDIR) \
		--search-path $(INCDIR)/openenclave/edl/sgx
	$(CC) -g -c $(CFLAGS) host.c
	$(CC) -g -c $(CFLAGS) helloworld_u.c
	$(CC) -o helloworld_host helloworld_u.o host.o $(LDFLAGS)

clean:
	rm -f helloworld_host host.o helloworld_u.o helloworld_u.c helloworld_u.h helloworld_args.h
