#!/bin/bash

cd openenclave

echo "=== patching AEP/TCS/EBASE ==="
patch -p1 < ../0001-Minimal-SGX-Step-bindings.patch
