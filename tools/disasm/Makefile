ZYDIS           = zydis-amalgamated-v4.0.0
SOURCES         = $(shell ls *.c $(ZYDIS)/*.c)
OBJECTS         = $(SOURCES:.c=.o)
CFLAGS          += -I$(ZYDIS)

all: disasm

disasm: $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

test: all
	cat test.txt | ./disasm.py
	@echo "-------------------"
	cat test.txt | ./disasm
