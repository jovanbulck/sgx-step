set -e
#git clone https://github.com/intel/linux-sgx.git
cd linux-sgx
./download_prebuilt.sh
sudo apt-get install build-essential ocaml automake autoconf libtool wget python
sudo apt-get install libssl-dev libcurl4-openssl-dev protobuf-compiler libprotobuf-dev
sudo apt-get install build-essential python
sudo apt-get install libssl-dev libcurl4-openssl-dev libprotobuf-dev
sudo apt-get install alien
#sudo alien --scripts iclsClient-1.45.449.12-1.x86_64.rpm
#sudo dpkg -i iclsclient_1.45.449.12-2_amd64.deb
sudo apt-get install uuid-dev libxml2-dev cmake pkg-config
#sudo apt install ocamlbuild

make sdk_install_pkg
make psw_install_pkg

cd linux/installer/bin/
sudo ./sgx_linux_x64_psw_*.bin
sudo ./sgx_linux_x64_sdk_*.bin << EOF
no
/opt/intel
EOF

echo "SGX SDK succesfully installed!"
