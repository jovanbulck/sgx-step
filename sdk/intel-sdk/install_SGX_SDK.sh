#!/bin/bash
set -e

## helper function to supress output to fit in CI max log length
function ci_silent() {
    if [ -n "$CI" ]; then
        ("$@" > ci_out.txt) || (\
          echo "FAIL: command '$@' returned with status $?; dumping stdout"; \
          echo "--------------------------------------------------------------------------------"; \
          tail -n 100 ci_out.txt; \
          echo "--------------------------------------------------------------------------------"; \
          exit 1 )
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
sudo apt-get -yqq install build-essential ocaml ocamlbuild automake autoconf libtool wget python2 libssl-dev git cmake perl
sudo apt-get -yqq install libssl-dev libcurl4-openssl-dev protobuf-compiler libprotobuf-dev debhelper cmake reprepro unzip lsb-release

OS_ID=$(lsb_release -si | tr '[:upper:]' '[:lower:]')
OS_REL=$(lsb_release -sr)
OS_STR=$OS_ID$OS_REL

if [ $OS_STR = "ubuntu22.04" ]; then
       echo "Warning: Ubuntu 22.04 LTS currently not yet officially supported by linux-sgx Intel SDK; overriding to Ubuntu 20.04 .."
       OS_STR="ubuntu20.04"
       sudo ln -fs /usr/bin/python2 /usr/bin/python
       python --version
fi

# ----------------------------------------------------------------------
echo "[ building SDK ]"
cd linux-sgx
make preparation
sudo cp "external/toolset/$OS_STR/"* /usr/local/bin

ci_silent make -j`nproc` sdk_install_pkg

echo "[ installing SDK system-wide ]"
cd linux/installer/bin/
sudo ./sgx_linux_x64_sdk_*.bin << EOF
no
/opt/intel
EOF
cd ../../../

# ----------------------------------------------------------------------
echo "[ building PSW ]"
ci_silent make -j`nproc` psw_install_pkg

echo "[ installing PSW/SDK system-wide ]"
cd linux/installer/bin/

if [ -e /opt/intel/sgxpsw/uninstall.sh ]
then
    sudo /opt/intel/sgxpsw/uninstall.sh
fi
sudo ./sgx_linux_x64_psw_*.bin

echo "SGX SDK succesfully installed!"
