#!/bin/bash

cd linux-sgx

echo "=== patching AEP ==="
patch -p1 < ../0001-Support-to-reconfigure-Asynchronous-Exit-Pointer-AEP.patch

echo "=== patching TCS ==="
patch -p1 < ../0002-Add-support-to-retrieve-most-recently-used-TCS-point.patch
