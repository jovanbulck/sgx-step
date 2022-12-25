# Open Enclave and SGX-Step Integration Demo (Ubuntu 18)

## Configure the Intel and Microsoft APT repositories

```sh
echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu bionic main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -
echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-7 main" | sudo tee /etc/apt/sources.list.d/llvm-toolchain-bionic-7.list
wget -qO - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
echo "deb [arch=amd64] https://packages.microsoft.com/ubuntu/18.04/prod bionic main" | sudo tee /etc/apt/sources.list.d/msprod.list
wget -qO - https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
sudo apt update
```

## Run the CI scripts 

```sh
cd sdk/oe-sdk/openenclave
sudo scripts/ansible/install-ansible.sh
sudo ansible-playbook scripts/ansible/oe-contributors-setup.yml
```

> **NOTE**:
> run `oe-contributors-setup-sgx1.yml` when the cpu with SGX 1 support. 

Before installing Open Enclave, make sure that the driver has been installed. 

To determine that, run `lsmod | grep -i sgx` or `dmesg | grep -i sgx`

> **NOTE**:
> The SGX DCAP driver has been deprecated, see how to transition from DCAP driver to kernel <https://github.com/intel/SGXDataCenterAttestationPrimitives/blob/master/driver/linux/README.kernel.md>
> - Intel SGX DCAP driver (deprecated) `intel_sgx`
> - Intel SGX out-of-tree driver (deprecated) `isgx`
> - Intel SGX in-kernel driver `sgx_enclave`


## Install Open Enclave 

```sh
cd sdk/oe-sdk
./install_OE_SDK.sh
```

## Build the SGX-Step Library 

```sh
cd libsgxstep
make
```

# OE Environment Variables 

```sh
cd sdk/oe-sdk
source oe-env.sh
```

# Build and Run a Hello World Program 

```sh
# build the hello world 
cd app-oe/aep-helloworld
mkdir build && cd build 
cmake .. 
make
# execute the hello world
make run
```
