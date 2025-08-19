#!/bin/bash

set -e
set -u

ROOT_DEV=$(findmnt -n -o SOURCE /)

echo "performing fsck on $ROOT_DEV..."
fsck -y "$ROOT_DEV"

if [ $? -eq 0 ]; then
    echo "fsck did not return errors, remounting to rw"
    mount -o remount,rw /
    
    echo "mounting other devices from /etc/fstab"
    mount -a
    
    echo "all filesystems got mounted"
else
    echo "fsck returned errors. filesystems may be corrupted"
    exit 1
fi

