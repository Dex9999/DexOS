#!/bin/sh
export PATH="$PATH:/home/bossmonster/opt/cross/bin"
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/DexOS.kernel isodir/boot/DexOS.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "DexOS" {
	multiboot /boot/DexOS.kernel
}
EOF
grub-mkrescue -o DexOS.iso isodir
