#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <Zydis.h>

/* TODO this should probably be an automatic symlink or so */
#define BINARY            "./Enclave/encl.so"
#define DEBUG             0

#define UNKNOWN           "???"
#define X86_MAX_INST_LEN  16
#define ZYDIS_DISASM      ZydisDisassembleATT
//#define ZYDIS_DISASM      ZydisDisassembleIntel

#if DEBUG
    #define debug(s, ...)     fprintf(stderr, __FILE__": " s, ##__VA_ARGS__)
#else
    #define debug(s, ...)
#endif

#define NEXT_LINE_ON(cond)                                              \
        if ((cond))                                                     \
        {                                                               \
	    output(UNKNOWN);                                            \
            continue;                                                   \
        }                                                               \

void output(const char *line)
{
    debug("outputing \"%s\"\n", line);
    printf("%s\n", line);
    fflush(stdout);
}

void supper(char *s)
{
    for (int i = 0; s[i]; i++)
        s[i] = toupper(s[i]);
}

int main()
{
    char line[1024];
    FILE *file = fopen(BINARY, "rb");
    if (!file)
    {
        perror("could not open file '" BINARY "'");
        /* can't exit here so GTKWave doesn't block */
    }

    while (fgets(line, sizeof(line), stdin) != NULL)
    {
        /* Expect a hexadecimal eRIP value from GTKWave */
        char *endptr = NULL;
        long erip = strtol(line, &endptr, 16);

        NEXT_LINE_ON(*endptr != '\0' && *endptr != '\n');
        debug("parsing enclave RIP=%#lx\n", erip);

        /* Read the raw bytes from the enclave binary */
        char data[X86_MAX_INST_LEN] = {0};
        NEXT_LINE_ON( !file );
        NEXT_LINE_ON( fseek(file, erip, SEEK_SET) != 0 );
        NEXT_LINE_ON( fread(data, 1, X86_MAX_INST_LEN, file) != X86_MAX_INST_LEN );

        #if DEBUG
            debug("Read raw data: ");
            for (int i=0; i < X86_MAX_INST_LEN; i++)
                fprintf(stderr, "%02x", *((uint8_t*) data + i));
            fprintf(stderr, "\n");
        #endif

        /* Pass it on to the zydis disassembler */
        ZydisDisassembledInstruction instruction;
        ZYDIS_DISASM(
            /* machine_mode:    */ ZYDIS_MACHINE_MODE_LONG_64,
            /* runtime_address: */ erip,
            /* buffer:          */ data,
            /* length:          */ X86_MAX_INST_LEN,
            /* instruction:     */ &instruction
        );
        debug("asm=\"%s\" (len=%d)\n", instruction.text, instruction.info.length);

        supper(instruction.text);
        output(instruction.text);
    }

    return 0;
}

