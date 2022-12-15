#!/bin/bash

echo "=== patching AEP/TCS ==="
patch -p1 < ../0001-SGX-Step-Gramine-patches-to-reconfigure-AEP-TCS.patch
