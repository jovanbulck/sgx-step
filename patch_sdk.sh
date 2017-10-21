#!/bin/bash

echo "=== checking out SGX SDK v1.9 ==="
cd linux-sgx
git checkout sgx_1.9

echo "=== patching AEP ==="
patch -p1 < ../0001-Support-to-reconfigure-Asynchronous-Exit-Pointer-AEP.patch

echo "=== patching TCS ==="
patch -p1 < ../0002-Add-support-to-retrieve-most-recently-used-TCS-point.patch
