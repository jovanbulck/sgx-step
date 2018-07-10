### 0. Setup

To configure the build processes for 32-bit, export the `M32` variable
as follows. Note that this variable should also be set in order to build
32-bit versions of the applications in `app/`.

```bash
export M32=1
``` 

### 1. Apply the patches to the SGX-SDK:

```bash
./patch_sdk.sh
```

While the Linux SGX-SDK is equipped to produce a 32-bit SDK installation
package, there are some small errors and warnings that need to be suppressed
for it to build. An extra SDK patch fixes these.

### 2. Build the SGX-SDK installation package

32-bit enclaves require a 32-bit SDK to function. The requirements to build 
are the same as described in the [linux-sgx](https://github.com/01org/linux-sgx) 
repository, but should be adapted to 32-bit:

Unless already installed, you will need to install gcc-multilib as well
as g++-multilib:
```bash
sudo apt install gcc-multilib g++-multilib
```
You will also need 32-bit versions of the required libraries
mentioned in the linux-sgx repo:
```bash
sudo apt install libssl-dev:i386 libcurl4-openssl-dev:i386 protobuf-compiler:i386 libprotobuf-dev:i386
```
You can then build the SGX-SDK install package with:
```bash
cd linux-sgx
make sdk_install_pkg ARCH=x86
```

### 3. Install the SGX-SDK installation package

The location of the installer will be shown at the end of the
compilation in the previous step.

Install the SDK to some location that is preferably **not** /opt/intel

### 4. Install the PSW package

Download and install the **unmodified** 64-bit PSW installation package.
If you have the newest PSW installed already, there is no need
to modify your installation. 

### 5. Build `libsgx_urts.so` and `libsgx_uae_service.so`

The build requirements are the same as for the 32-bit SDK. We will
not be building a full psw install package, we just need the 
two mentioned libraries.

```bash
cd psw/urts/linux
make all ARCH=x86
```

When the build finishes, the required libraries are in the following directories:
```bash
psw/urts/linux/libsgx_urts.so
psw/uae_service/linux/libsgx_uae_service.so
```

To make sure the linker finds the libraries when compiling the applications in
app/, move the uae_service library to the urts library location:
```bash
cd ../../../ 
mv psw/uae_service/linux/libsgx_uae_service.so psw/urts/linux
```

If your SDK installation path was *not* /opt/intel, then the makefiles for the sgx-step apps will find the libraries automatically. Otherwise, you can either move the libraries to /usr/lib, or set up your environment variables to point to a directory containing the libraries.