#!/bin/bash

echo "=== patching LIBSGXSTEP ==="
patch -p1 < ../0000-Libsgxstep-patches.patch
