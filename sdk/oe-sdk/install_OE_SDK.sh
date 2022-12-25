#!/bin/bash
set -e

# ----------------------------------------------------------------------
echo "[ downloading Open Enclave SDK third party libraries ]"
git submodule update --init --recursive --progress

# ----------------------------------------------------------------------
echo "[ patching Open Enclave SDK ]"
if ! grep -Rq "sgx_set_aep" openenclave
then
    ./patch_oe_sdk.sh
fi
echo "OE-SDK successfully patched!"

# ----------------------------------------------------------------------
echo "[ building the Open Enclave SDK]"
echo "NOTE: the cmake configuration may require network access, it may stuck at a certain stage."

mkdir openenclave/build

pushd openenclave/build
cmake .. -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TESTS=off
make
make install
popd


# ----------------------------------------------------------------------
CURR_DIR=$(pwd)
SGX_STEP_HOME=$(builtin cd $CURR_DIR/../..; pwd)
echo "current directory is $SGX_STEP_HOME"
# ----------------------------------------------------------------------
echo "[ generating environment variables ]"
cat > ./oe-env.sh << EOF
source $(pwd)/openenclave/install/share/openenclave/openenclaverc
export SGX_STEP_INCLUDE=$SGX_STEP_HOME/libsgxstep
export SGX_DRIVER_DIR=$SGX_STEP_HOME/kernel/linux-sgx-driver
export OE_HOME=$(pwd)/openenclave/install
export OE_BIN=$(pwd)/openenclave/install/bin
EOF

# ----------------------------------------------------------------------
echo "-----------------------------------------------------------------"
printf "\n\n\e[1;33mNOTE\e[0m: add the following line to ~/.bashrc\n\n"
printf "\t \e[1;32m source $(pwd)/oe-env.sh \e[0m\n\n"

