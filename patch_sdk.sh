#!/bin/bash

cd linux-sgx

if [[ -v M32 ]]
then
    echo "=== patching SDK   ==="
    patch -p1 < ../0000-32bit-compatibility-fixes.patch
fi

echo "=== patching AEP/TCS/EBASE ==="
patch -p1 < ../0001-reconfigure-AEP-TCS-ebase.patch
