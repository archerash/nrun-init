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

depends=("gcc" "mv" "cp" "mkdir" "make")

for i in "${depends[@]}"; do
  check_depends $i
done

if [ $fail == 1 ]; then
  exit 1
fi

mkdir bin
make

mkdir -p /etc/re/core-services
mkdir -p /var/sv

mv bin/re /usr/bin
mv bin/rectl /usr/bin
mv bin/mountall /usr/bin

cp core-services/01-udevd.sh /etc/re/core-services
cp core-services/02-filesystems.sh /etc/re/core-services
cp core-services/03-hostname.sh /etc/re/core-services

cp stages/* /etc/re

cp services/* /var/sv

chmod +x /var/sv/*
chmod +x /etc/re/{1,2,3}
chmod +x /etc/re/core-services/*
