#!/bin/bash

cd gramine

echo "=== patching ECALL ==="
patch -p1 < ../0002-Example-usage-of-libsgxstep-functionality-on-Gramine.patch
