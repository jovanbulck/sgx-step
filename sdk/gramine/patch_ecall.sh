#!/bin/bash

echo "=== patching ECALL ==="
patch -p1 < ../0002-helloworld-Nemesis.patch
