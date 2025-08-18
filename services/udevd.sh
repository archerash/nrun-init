#!/bin/sh
modprobe hid_generic
modprobe usbhid
if ! pgrep -x udevd >/dev/null; then
  echo "Starting udevd..."
  /sbin/udevd --daemon
fi
udevadm trigger --subsystem-match=input --action=add
udevadm settle
echo "Detected input devices: "
ls -l /dev/input
chgrp input /dev/input/event* 2>/dev/null
chmod 660 /dev/input/event* 2>/dev/null
