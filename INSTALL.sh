#!/bin/bash

if [ $EUID -ne 0 ]; then
  echo "Run this file as root!"
fi

echo "Compiling RE init and useful utils"

mkdir bin
gcc src/*.c src/include/* -o bin/re

gcc src/utils/poweroff.c -o bin/re-poweroff
gcc src/utils/reboot.c -o bin/re-reboot

echo "Creating dirs for Re to work and moving files"

mkdir -p /var/sv
mkdir -p /etc/re/core-services

mv bin/re /usr/bin
mv bin/re-poweroff /usr/bin
mv bin/re-reboot /usr/bin

echo "Adding basic core services"
cp core-services/01-udevd.sh /etc/re/core-services
cp core-services/03-hostname.sh /etc/re/core-services

read -p "Do you want to use simple shell script or more advanced binary to mount your devices from /etc/fstab? Type 1 for shell script and 2 for binary" dec

if [ $dec == 1 ]; then
  cp core-services/02-filesystems.sh /etc/re/core-services
else
  gcc src/mount/*.c src/mount/include/* -o bin/filesystems.bin
  mv bin/mountall.bin /etc/re/core-services/
fi

cp services/udevd.sh services/agetty-tty1.sh /var/sv
chmod +x /var/sv/*
chmod +x /etc/re/core-services/*

echo "Re is now installed. You can edit your GRUB and boot into it..."

exit 0
