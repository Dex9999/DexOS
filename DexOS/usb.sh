#!/bin/bash

echo "Finding USB devices..."

lsblk -o NAME,SIZE,TYPE,MOUNTPOINT,VENDOR,MODEL,TRAN | grep "disk\|part" | while read -r line; do
    name=$(echo "$line" | awk '{print $1}')
    size=$(echo "$line" | awk '{print $2}')
    type=$(echo "$line" | awk '{print $3}')
    mountpoint=$(echo "$line" | awk '{print $4}')
    vendor=$(echo "$line" | awk '{print $5}')
    model=$(echo "$line" | awk '{print $6}')
    transport=$(echo "$line" | awk '{print $7}')

    if [[ $model == "usb" || $model == "Flash-Disk" ]]; then
        echo "-------------------------------------------"
        echo "Device:   $name"
        echo "Size:     $size"
        echo "Type:     $type"
        echo "Vendor:   $vendor"
        echo "Model:    $model"
        echo "Mountpoint: $mountpoint"
        echo "-------------------------------------------"
        
        read -p "Are you ABSOLUTELY SURE this is the right device? (y/n): " answer </dev/tty
        if [[ $answer == "n" ]]; then
            echo "You can use lsblk or the file explorer to be sure :)"
            exit 1
        fi
        read -p "iso file name: " iso </dev/tty
        if [[ $answer == "y" ]]; then
            echo "Writing..."
            sudo dd if=$iso of=/dev/$name && sync
            echo "Have fun with your usb!!!"
            exit 0
        fi
    fi
done