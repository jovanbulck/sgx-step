#!/bin/bash
set -e

git submodule init
git submodule update

# ----------------------------------------------------------------------
echo "[ patching SDK ]"
if ! grep -Rq "sgx_set_aep" linux-sgx
then
    ./patch_sdk.sh
fi
echo "SGX-SDK successfully patched!"

# ----------------------------------------------------------------------
echo "[ installing prerequisites ]"
sudo apt-get install build-essential ocaml ocamlbuild automake autoconf libtool wget python libssl-dev
sudo apt-get install libssl-dev libcurl4-openssl-dev protobuf-compiler libprotobuf-dev debhelper cmake

# ----------------------------------------------------------------------
echo "[ building SDK ]"
cd linux-sgx
./download_prebuilt.sh
make -j`nproc`
make sdk_install_pkg

echo "[ installing SDK system-wide ]"
cd linux/installer/bin/
sudo ./sgx_linux_x64_sdk_*.bin << EOF
no
/opt/intel
EOF
cd ../../../

# ----------------------------------------------------------------------
echo "[ building PSW ]"
make psw_install_pkg

echo "[ installing PSW/SDK system-wide ]"
cd linux/installer/bin/

if [ -e /opt/intel/sgxpsw/uninstall.sh ]
then
    sudo /opt/intel/sgxpsw/uninstall.sh
fi
sudo ./sgx_linux_x64_psw_*.bin

echo "SGX SDK succesfully installed!"
