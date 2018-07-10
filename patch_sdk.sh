#!/bin/bash

cd linux-sgx

if [[ -v M32 ]]
then
    echo "=== patching SDK   ==="
    patch -p1 < ../0000-32bit-compatibility-fixes.patch
fi

echo "=== patching AEP   ==="
patch -p1 < ../0001-Support-to-reconfigure-Asynchronous-Exit-Pointer-AEP.patch

echo "=== patching TCS   ==="
patch -p1 < ../0002-Add-support-to-retrieve-most-recently-used-TCS-point.patch

echo "=== patching EBASE ==="
patch -p1 < ../0003-Support-to-set-enclave-load-location.patch
