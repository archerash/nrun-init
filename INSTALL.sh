#!/bin/bash

if [ $EUID -ne 0 ]; then
  echo "Run this file as root"
fi

fail=0

check_depends() {
  if ! command -v $1 >/dev/null; then
    echo "$1 is not installed"
    fail=1
  fi
}

depends=("gcc" "mv" "cp" "mkdir")

for i in "${depends[@]}"; do
  check_depends $i
done

if [ $fail == 1 ]; then
  exit 1
fi

echo "Stage 1: Compiling..."

mkdir bin
gcc src/*.c src/include/*.h -o bin/re
gcc src/poweroff/* -o /bin/re-poweroff
gcc src/reboot/* -o bin/re-reboot

echo "Stage 2: Installing init..."

mkdir -p /var/sv
mkdir -p /etc/re/core-services

echo "Installing init, poweroff and reboot"

mv bin/re /usr/bin
mv bin/re-poweroff /usr/bin
mv bin/re-reboot /usr/bin

read -p "Would you like to use shell script or C implementation of mountall? Type 1 for shell and 2 for C" dec

if [ $dec == 1]; then
  cp core-services/02-filesystems.sh /etc/re/core-services
else
  gcc src/mountall/*.c src/mountall/include/*.h -o bin/mountall
  mv bin/mountall /etc/re/core-services
fi

echo "Stage 3: Installing core services..."

cp core-services/01-udevd.sh /etc/re/core-services
cp core-services/03-hostname.sh /etc/re/core-services

echo "Stage 4: Installing post-mount services..."

cp services/agetty-tty1.sh /var/sv
cp services/udevd.sh /var/sv

echo "Stage 5: Finalizing..."

chmod +x /var/sv/*
chmod +x /etc/re/core-services/*

echo "Now. When everything is installed. Go to your /etc/default/grub, add init=/usr/bin/re as an argument to GRUB_CMDLINE_LINUX_DEFAULT line and reboot"