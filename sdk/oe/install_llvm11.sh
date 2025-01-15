#!/bin/bash
#set -x

# NOTE: 20.04 tarball also works on 24.04
TAR_BASE="clang+llvm-11.0.0-x86_64-linux-gnu-ubuntu-20.04"
TAR_FILE="$TAR_BASE.tar.xz"
TAR_URL="https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/$TAR_FILE"
TMP_DIR="/tmp/llvm-temp"
TARGET_BASE="/usr"

mkdir -p "$TMP_DIR"
cd $TMP_DIR
if [ ! -e $TAR_FILE ]; then
    wget $TAR_URL
    tar xvf $TAR_FILE
fi

sudo mkdir -p /usr/lib/llvm-11/
sudo cp -r $TMP_DIR/$TAR_BASE/* /usr/lib/llvm-11/

echo "Extraction and moving completed."
