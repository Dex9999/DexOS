#!/bin/bash

# os.bin path
MYOS_BINARY_PATH="DexOS.bin"

# 40_custom path
GRUB_CFG_PATH="/etc/grub.d/40_custom"

# Boot dir
BOOT_DESTINATION="/boot/"

# Root partition 
ROOT_PARTITION="hd0,msdos5"

sudo sed -i '/^menuentry '\''DexOS'\'' {/,/^}/d' "$GRUB_CFG_PATH"

# Copy os.bin to the boot directory
sudo cp "$MYOS_BINARY_PATH" "$BOOT_DESTINATION"

# Add menuentryentry to grub.cfg
echo "menuentry 'DexOS' {" | sudo tee -a "$GRUB_CFG_PATH"
echo "    set root='$ROOT_PARTITION'" | sudo tee -a "$GRUB_CFG_PATH"
echo "    multiboot /boot/$(basename $MYOS_BINARY_PATH)" | sudo tee -a "$GRUB_CFG_PATH"
echo "}" | sudo tee -a "$GRUB_CFG_PATH"

sudo update-grub

echo "You can now run from boot!"
