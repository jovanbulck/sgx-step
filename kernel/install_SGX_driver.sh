#!/bin/bash

set -e

if [ -e /dev/sgx_enclave ]; then
    echo "Found in-kernel /dev/sgx_enclave driver; skipping installation of out-of-tree isgx driver!"
    exit 0
fi

if [ -e /dev/isgx ]; then
    echo "Found pre-installed out-of-tree /dev/isgx driver!"
    exit 0
fi
echo "Building and installing out-of-tree isgx driver.."

git submodule init
git submodule update
cd linux-sgx-driver

# ----------------------------------------------------------------------
sudo apt-get -yqq install linux-headers-$(uname -r)
make
sudo mkdir -p "/lib/modules/"`uname -r`"/kernel/drivers/intel/sgx"    
sudo cp isgx.ko "/lib/modules/"`uname -r`"/kernel/drivers/intel/sgx"    
sudo sh -c "cat /etc/modules | grep -Fxq isgx || echo isgx >> /etc/modules"    
sudo /sbin/depmod
sudo /sbin/modprobe isgx
sudo dmesg | tail

echo "SGX driver succesfully installed"
