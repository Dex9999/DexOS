# $HOME is finicky, so i'm using /home/username
export START="/home/bossmonster"
export PREFIX="$START/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
echo $PREFIX
echo $TARGET
echo $PATH
export PATH="$START/opt/cross/bin:$PATH"
export assembler=1

# i'll figure out ~./profile later
if [ "$assembler" = 1 ]; then
    nasm -felf32 boot.asm -o boot.o
fi
if [ "$assembler" = 0 ]; then
    i686-elf-as boot.s -o boot.o
fi

i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
if grub-file --is-x86-multiboot myos.bin; then
  echo multiboot confirmed
else
  echo the file is not multiboot
fi
mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/myos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o myos.iso isodir
qemu-system-i386 -cdrom myos.iso