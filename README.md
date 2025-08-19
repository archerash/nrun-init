# RE - Lightweight and minimalist init system for Linux

Re is simple, lightweight and minimalist init system designed mainly for Linux systems. Re should be started as first PID for it to work.

## How does Re work?

Re works by opening directories and running services in these directories. First it opens `/etc/re/core-services` and runs all executable files there (stage 1), then it enters stage 2 in which it runs all executable files in `/var/sv`. When it runs all of the services it enters stage 3 in which it awaits signals. If it receives `SIGCHLD` signal it cleans up finished process (zombie process). If it receives `SIGTERM` it shutdowns OS and for `SIGINT` it reboots.

To add a new service to Re simple create shell script in `/var/sv` directory with [shebang](https://en.wikipedia.org/wiki/Shebang_(Unix)) at the top and code you want to run each time your OS boots. More important services that ensure basic operation of the OS should be added to `/etc/re/core-services` directory.

## How to install Re?

Installation of Re is quite simple. There is a file named `INSTALL.sh`, you can run it if you want to install re automatically, here is manual guide:

1. Clone this repo:
```
cd $HOME/
git clone https://github.com/zerfithel/re
cd re
```

2. Compile:
```
mkdir bin
make
```

3. Create re directories:
```
sudo mkdir -p /var/on
sudo mkdir -p /etc/re/core-services
```

4. Install files into desired folders:
```
sudo mv bin/re /usr/bin/re
sudo mv bin/rectl /usr/bin/re
sudo mv bin/mountall /usr/bin/re
```

5. Add services and stage scripts:
```
sudo cp stages/{1,2,3} /etc/re
sudo cp services/agetty-tty1.sh /var/sv/agetty-tty1
sudo cp services/udevd.sh /var/sv/udevd
```

6. Add core services:
```
sudo cp core-services/01-udevd.sh /etc/re/core-services
sudo cp core-services/02-filesystems.sh /etc/re/core-services
sudo cp core-services/03-hostname.sh /etc/re/core-services
```

You can change `mount -a` line in `02-filesystems.sh` with `mountall` - re implementation of mount all. It should be in `bin/mountall` directory (after compiling and creating `bin/` directory)

7. Boot into OS with `re` as init system once:

Now, reboot your system and in GRUB when targeting label to start your OS click `e` and go to line that begins with `linux`. At the end of the line add `init=/usr/bin/re` and remove `ro` if its in same line. You can add `quiet` to make kernel not show useless logs.

Then, just press `CTRL-X` and wait for OS to start. If you can see all logs indicating services in both `/etc/re/core-services` and `/var/sv` are being run. If you added `tty.sh` as a service try to login with your user account and see if your devices from `/etc/fstab` are mounted. For example if you have a separate /boot partition run the following command: `ls /boot`. If you can see all files there, it means your devices are mounted and you can use your OS as usual.

To make `re` your default init system run:
```re-reboot```
and proceed to the next step

8. Set `re` as your default init system

In order to do that, just edit your `/etc/default/grub` configuration file and in line that begins with `GRUB_CMDLINE_LINUX_DEFAULT` add as an argument `init=/usr/bin/re`
Example line:
```
GRUB_CMDLINE_LINUX_DEFAULT="quiet init=/usr/bin/re"
```

Then, rebuild your GRUB configuration and reboot:
```
sudo grub-mkconfig -o /boot/grub/grub.cfg
sudo reboot
```

When you will see `re` logs and no errors you can use your OS with `re`. To ensure you are running only `re` as init system run the following command:
```
ps aux | grep re
```

If PID 1 is `re` you are successfuly running `re` init system on your machine. To reboot use `re-reboot` and to shutdown run `re-poweroff`. If you didnt install these utils for some weird reason you can use `kill -s SIGINT 1` to reboot and `kill -s SIGTERM 1` to shutdown.

## How does Re work?

Re is very similar to `runit`. It works by executing `/etc/re/1`, then `/etc/re/2` at boot and `/etc/re/3` at shutdown. In default Re configuration `/etc/re/1` executes all services in `/etc/re/core-services` directory, then `/etc/re/2` executes `/var/sv/` services. All "stages scripts" (`/etc/re/{1,2,3}`) are just shell scripts but can be replaced with binaries. You can edit these to edit how init behaves without need to recompile. For minimal system configuration you can copy stages from `stages/` directory in this repository. Paste these files into your `/etc/re/`. Also, you can copy services from `core-services/` and copy these to `/etc/re/core-services`. If you want to, you can compile "mountall" from this directory - `src/init/mount` and replace `mount -a` in `/etc/re/core-services/02-filesystems` with `mountall`.

## How to use Rectl?

Rectl is CLI util to shutdown, reboot, start and monitor service and run service directory. Here are some examples of its usage:

`rectl poweroff` - Shutdown now

`rectl reboot` - Reboot now

`rectl svdir /var/sv/` - Start all services in `/var/sv/` directory

`rectl sv 10 1 elogind` - Start `elogind` and monitor it. If it fails sleep for 1 second and attempt to start it again 10 times.

## Wiki

I suggest reading Re wiki. It will take you less than 10 minutes to read and understand how Re works and how to use it.

[re/configuration](https://zerfithel.github.io/software/re/configuration) - Learn how to configure Re. This includes: running graphical session, running pipewire, enabling your drivers etc.

[re/rectl](https://zerfithel.github.io/software/re/rectl) - Learn how to use rectl (manage services, edit stage scripts and more).

[re/pipewire](https://zerfithel.github.io/software/re/pipewire) - Full guide on how to run pipewire on Re without any issues.

[re/howitworks](https://zerfithel.github.io/software/re/howitworks) - Learn how `re` works. This is mostly for people that are interested how it works under the mask.

[re/news](https://zerfithel.github.io/software/re/news) - News about Re.

## How to uninstall Re?

Uninstalling Re is very simple. You just remove `init=/usr/bin/re` from your `/etc/default/grub` file. When you do that, run the following command:
```
sudo grub-mkconfig -o /boot/grub/grub.cfg
```

But what if your OS does not boot up or you cant edit your filesystem? Then, you have to create a bootable USB with tools like Etcher or Ventoy. [Etcher Guide](https://www.how2shout.com/how-to/balenaetcher-how-to-create-a-bootable-usb-flash-drive-using-etcher.html).

If your bootable USB is ready you can plug it in and choose it as your #1 boot option. Wait for it to load and open your tty/terminal inside your live system.

Then run the following command and indentificate your disk:
```
lsblk
```

If you can see your disk (look at disk size/partitions etc.) mount it using `mount` command.
For example, if your disk configuration looks like this:
```
├─sdb1   8:17   0     1G  0 part /boot/efi
├─sdb2   8:18   0    50G  0 part /
├─sdb3   8:19   0   330G  0 part /home
```

Run the following commands:
```
sudo mkdir /mnt
sudo mount /dev/sdb2 /mnt/
sudo mount /dev/sdb1 /mnt/boot/efi
sudo mount /dev/sdb3 /mnt/home
```

Its important that root (/) partition should be mounted first.

Then, use tool like `chroot` to change your root into your mounted filesystem:
```
sudo chroot /mnt
```

There, edit with your text editor `/etc/default/grub` file and remove `init=/usr/bin/re` in `GRUB_CMDLINE_LINUX_DEFAULT` line, then save the file and run the following command:
```
sudo grub-mkconfig -o /boot/grub/grub.cfg
```
After that, you can exit your mounted filesystem and reboot:
```
exit
sudo umount -a
sudo reboot
```

Unplug your USB and wait for your OS to boot. It should start with your default init system. If you did remove your old init system I suggest reinstalling your OS to avoid weird errors.

## Contact & Links

Discord: `@zerfithel`

Website: [zerfithel.github.io](https://zerfithel.github.io)
