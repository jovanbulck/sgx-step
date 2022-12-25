#!/bin/bash

echo "=== patching AEP/TCS/EBASE ==="
cd openenclave
patch -p1 < ../0001-oe-reconfigure-AEP-TCS-ebase.patch
