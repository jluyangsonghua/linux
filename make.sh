!bin/sh
gnu_dir=/home/ysh/code/toolchain/gcc-linaro-7.5.0-2019.12-rc1-x86_64_aarch64-linux-gnu/aarch64-linux-gnu
CROSS_COMPILE_DIR=$gnu_dir/bin/aarch64-linux-gnu-
make ARCH=arm64 CROSS_COMPILE=$CROSS_COMPILE_DIR menuconfig
