#!/bin/bash

set -e

git submodule init
git submodule update
cd linux-sgx-driver

# ----------------------------------------------------------------------
sudo apt-get install linux-headers-$(uname -r)
make
sudo mkdir -p "/lib/modules/"`uname -r`"/kernel/drivers/intel/sgx"    
sudo cp isgx.ko "/lib/modules/"`uname -r`"/kernel/drivers/intel/sgx"    
sudo sh -c "cat /etc/modules | grep -Fxq isgx || echo isgx >> /etc/modules"    
sudo /sbin/depmod
sudo /sbin/modprobe isgx

echo "SGX driver succesfully installed"
