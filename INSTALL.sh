#!/bin/bash

if [ $EUID -ne 0 ]; then
  echo "Run this file as root"
fi

fail=0

check_depends() {
  if ! command -v $1 >/dev/null; then
    echo "$1 is not installed!"
    fail=1
  fi
}

depends=("gcc" "make")

for i in "${depends[@]}"; do
  check_depends $i
done

if [ $fail == 1 ]; then
  exit 1
fi

make # compile

mkdir -p /etc/re/core-services # core services dir
mkdir -p /etc/re/sv # service dir
mkdir -p /var/sv # enabled services dir

chmod +x bin/*
mv bin/* /usr/bin

#stage 1
echo "#\!/bin/sh\necho '=> Re: Stage 1'\nexec rectl svdir /etc/re/core-services" >/etc/re/1
#stage 2
echo "#\!/bin/sh\necho '=> Re: Stage 2'\nexec rectl svdir /var/sv" >/etc/re/2

#stage 3 
echo "#\!/bin/sh\necho '=> Re: Stage 3'\nrehalt\nexec umount -a" >/etc/re/3 

chmod +x /etc/re/{1,2,3}

cp ./core-services/* /etc/re/core-services 
chmod +x /etc/re/core-services/*

#ttys
for i in {1..9}; do 
  echo "#\!/bin/sh\nexec agetty --noclear 38400 tty$i linux" >/etc/re/sv/agetty-tty$i  
  chmod +x /etc/re/sv/agetty-tty$i 
  ln -s /etc/re/sv/agetty-tty$i /var/sv
done

echo "Succes: Now edit GRUB config and add init=/usr/bin/re to GRUB_CMDLINE_LINUX_DEFAULT line, rebuild grub config and reboot! You should be greeted with tty login screen"
exit 0
