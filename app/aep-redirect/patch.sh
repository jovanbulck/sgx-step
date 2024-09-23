#!/bin/bash

# ENCLU        = 0F 01 D7
# UD2; NOP     = 0F 0B 90
# INT3;NOP;NOP = CC 90 90
enclu_replacement='\x0f\x0b\x90'
enclu_replacement='\xcc\x90\x90'
patch_dir="./patched-libs"

if [[ -z $1 ]];
then 
    echo "Usage:" $0 "./app"
    exit 1
fi

printf ".. parsing shared library dependencies for '%s'\n" $1
SHLIBS=$(ldd $1 | sed -En 's/(.*) => (.*) \(.*\)/\2/p')

for lib in $SHLIBS
do
    printf ".. checking shared library dependency %-50s" $lib
    ENCLU=$(objdump -d $lib | grep enclu | sed -En 's/\s*([[:xdigit:]]*):.*/\1/p')
    if [[ -z $ENCLU ]]; then
        echo -e " [OK]"
    else
        echo -e " [Found ENCLU]"
        
        echo -e "\t.. creating local patched library copy"
        mkdir -p $patch_dir
        cp $lib $patch_dir
        dest_lib=$patch_dir/$(basename $lib)

        for hexAdrs in $ENCLU
        do
            decAdrs=$(( 16#$hexAdrs ))
            printf "\t.. patching enclu at adrs 0x%s (decimal %d)\n" $hexAdrs $decAdrs
            printf $enclu_replacement | dd of=$dest_lib bs=1 seek=$decAdrs count=3 conv=notrunc
        done
        printf "\t.. local patched library copy available at '%s'\n" $dest_lib
    fi
done
printf ".. patching done; run with LD_LIBRARY_PATH=%s %s\n" $patch_dir $1
