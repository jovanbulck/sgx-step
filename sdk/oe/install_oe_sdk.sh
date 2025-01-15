#!/bin/bash
set -e

git submodule init
git submodule update --recursive --init

# ----------------------------------------------------------------------
echo "[ patching SDK ]"
if ! grep -Rq "sgx_set_aep" openenclave
then
    ./patch_sdk.sh
fi
echo "OE SDK successfully patched!"

# ----------------------------------------------------------------------
echo "[ installing prerequisites ]"
./install_llvm11.sh

# ----------------------------------------------------------------------
echo "[ building OE SDK ]"
cd openenclave
mkdir -p build
cd build
cmake -DCMAKE_C_COMPILER=/usr/lib/llvm-11/bin/clang -DCMAKE_CXX_COMPILER=/usr/lib/llvm-11/bin/clang++ -DENABLE_REFMAN=OFF ..
make
sudo make install
source /opt/openenclave/share/openenclave/openenclaverc

echo "OE SDK succesfully installed!"
