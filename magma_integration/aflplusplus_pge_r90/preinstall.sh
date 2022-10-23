#!/bin/bash
set -e

apt-get update && \
    apt-get install -y make lsb-release software-properties-common gnupg \
        build-essential git wget gcc-7-plugin-dev

wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
./llvm.sh 13 all
rm llvm.sh

update-alternatives \
  --install /usr/lib/llvm              llvm             /usr/lib/llvm-13  20 \
  --slave   /usr/bin/llvm-config       llvm-config      /usr/bin/llvm-config-13  \
    --slave   /usr/bin/llvm-ar           llvm-ar          /usr/bin/llvm-ar-13 \
    --slave   /usr/bin/llvm-as           llvm-as          /usr/bin/llvm-as-13 \
    --slave   /usr/bin/llvm-bcanalyzer   llvm-bcanalyzer  /usr/bin/llvm-bcanalyzer-13 \
    --slave   /usr/bin/llvm-c-test       llvm-c-test      /usr/bin/llvm-c-test-13 \
    --slave   /usr/bin/llvm-cov          llvm-cov         /usr/bin/llvm-cov-13 \
    --slave   /usr/bin/llvm-diff         llvm-diff        /usr/bin/llvm-diff-13 \
    --slave   /usr/bin/llvm-dis          llvm-dis         /usr/bin/llvm-dis-13 \
    --slave   /usr/bin/llvm-dwarfdump    llvm-dwarfdump   /usr/bin/llvm-dwarfdump-13 \
    --slave   /usr/bin/llvm-extract      llvm-extract     /usr/bin/llvm-extract-13 \
    --slave   /usr/bin/llvm-link         llvm-link        /usr/bin/llvm-link-13 \
    --slave   /usr/bin/llvm-mc           llvm-mc          /usr/bin/llvm-mc-13 \
    --slave   /usr/bin/llvm-nm           llvm-nm          /usr/bin/llvm-nm-13 \
    --slave   /usr/bin/llvm-objdump      llvm-objdump     /usr/bin/llvm-objdump-13 \
    --slave   /usr/bin/llvm-ranlib       llvm-ranlib      /usr/bin/llvm-ranlib-13 \
    --slave   /usr/bin/llvm-readobj      llvm-readobj     /usr/bin/llvm-readobj-13 \
    --slave   /usr/bin/llvm-rtdyld       llvm-rtdyld      /usr/bin/llvm-rtdyld-13 \
    --slave   /usr/bin/llvm-size         llvm-size        /usr/bin/llvm-size-13 \
    --slave   /usr/bin/llvm-stress       llvm-stress      /usr/bin/llvm-stress-13 \
    --slave   /usr/bin/llvm-symbolizer   llvm-symbolizer  /usr/bin/llvm-symbolizer-13 \
    --slave   /usr/bin/llvm-tblgen       llvm-tblgen      /usr/bin/llvm-tblgen-13

update-alternatives \
  --install /usr/bin/clang                 clang                  /usr/bin/clang-13     20 \
  --slave   /usr/bin/clang++               clang++                /usr/bin/clang++-13 \
  --slave   /usr/bin/clang-cpp             clang-cpp              /usr/bin/clang-cpp-13
