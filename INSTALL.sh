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

mkdir bin
make

mkdir -p /etc/re/core-services
mkdir -p /etc/re/sv
mkdir -p /var/sv
touch /etc/re/{1,2,3}
chmod +x /etc/re/{1,2,3}

git clone https://github.com/zerfithel/dotfiles
cp dotfiles/re/sv/* /etc/re/sv
chmod +x /etc/re/sv/*
ln -s /etc/re/sv/* /var/sv

cp dotfiles/re/core-services/* /etc/re/core-services
chmod +x /etc/re/core-services/*

cp dotfiles/re/{1,2,3} /etc/re
chmod +x /etc/re/{1,2,3}

echo "success: installed: now follow guide from step 6! btw, thank you so much for trying out RE <3"
exit 0
