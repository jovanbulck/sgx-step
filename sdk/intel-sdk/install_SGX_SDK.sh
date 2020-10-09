#!/bin/bash
set -e

## helper function to supress output to fit in Travis-CI max log length
function travis_silent() {
    if [ -n "$TRAVIS" ]; then
        "$@" > /dev/null
    else
        "$@"
    fi
}

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
sudo apt-get -yqq install build-essential ocaml ocamlbuild automake autoconf libtool wget python libssl-dev git cmake perl
sudo apt-get -yqq install libssl-dev libcurl4-openssl-dev protobuf-compiler libprotobuf-dev debhelper cmake reprepro unzip lsb-release

OS_ID=$(lsb_release -si | tr '[:upper:]' '[:lower:]')
OS_REL=$(lsb_release -sr)
OS_STR=$OS_ID$OS_REL

# ----------------------------------------------------------------------
echo "[ building SDK ]"
cd linux-sgx
make preparation
sudo cp "external/toolset/$OS_STR/"* /usr/local/bin

travis_silent make -j`nproc` sdk_install_pkg

echo "[ installing SDK system-wide ]"
cd linux/installer/bin/
sudo ./sgx_linux_x64_sdk_*.bin << EOF
no
/opt/intel
EOF
cd ../../../

# ----------------------------------------------------------------------
echo "[ building PSW ]"
travis_silent make -j`nproc` psw_install_pkg

echo "[ installing PSW/SDK system-wide ]"
cd linux/installer/bin/

if [ -e /opt/intel/sgxpsw/uninstall.sh ]
then
    sudo /opt/intel/sgxpsw/uninstall.sh
fi
sudo ./sgx_linux_x64_psw_*.bin

echo "SGX SDK succesfully installed!"
