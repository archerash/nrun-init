# NRun - A lightweight and minimalist init system for Linux

**NRun is supposed to be ran as PID 1 process. If it wont, it will automatically quit and wont do anything.**

I am going to release NUtils package to manage and monitor services on NRun systems soon. I will post updates [here](https://zerfithel.github.io/news). 

## Warning ⚠️

I do NOT provide any kind of warranty for this software. Use it at your own risk. Keep in mind this is early development so it may break or not even run. If you find any bugs please report these to my Discord: `@zerfithel`.

## Installing & Running NRun 📥

This guide assumes you are using `GRUB` as your bootloader and you have `gcc` and `git` installed.

1. First, clone this repo:
```
cd $HOME/
git clone https://github.com/zerfithel/nrun-init
cd nrun-init
```

2. Then compile NRun:
```
mkdir bin
gcc src/*.c src/include/* -o bin/nrun
```

3. Move nrun binary to `/usr/bin`:
```
sudo mv bin/nrun /usr/bin/nrun
```

4. Create required directories for NRun to run and add example service:
```
sudo mkdir /var/on
sudo cp ./services-examples/agetty-tty1 /var/on
sudo chmod +x /var/on/agetty-tty1
```

5. Reboot your OS and click `e` when targeting label to start your OS:
```
reboot
```

7. Find line that begins with `linux` and add `init=/usr/bin/nrun` at the end.

8. Press `CTRL-X` and wait for system to boot. If it boots and you can login, you can make nrun run every boot by:
```
EDITOR="your_favourite_editor"
$EDITOR /etc/default/grub
```

8. Find line that begins with `GRUB_CMDLINE_LINUX_DEFAULT` and add `init=/usr/bin/nrun` at the end. Example line:
```
GRUB_CMDLINE_LINUX_DEFAULT="quiet init=/usr/bin/nrun"
```

9. Rebuild your GRUB configuration:
- If you are using Debian-based distro:
```
sudo update-grub
```
- If you arent using Debian-based distro:
```
sudo grub-mkconfig -o /boot/grub/grub.cfg
```

10. If everything works you can proceed with configuring your OS running NRun:

[nrun-init/configuration](https://zerfithel.github.io/software/nrun-init/configuration) - Learn how to configure NRun, so you can run graphical session, run GUI software or connect to the network.

[nrun-init/howitworks](https://zerfithel.github.io/software/nrun-init/howitworks) - Learn how NRun works and how to use it.

[nrun-init/nvidia](https://zerfithel.github.io/software/nrun-init/nvidia) - Learn how to use nvidia with nrun.

[nrun-init/services](https://zerfithel.github.io/software/nrun-init/services) - Take a look at useful services that you can copy to `/var/on` dir.

[news](https://zerfithel.github.io/news) - News about nrun-init. I suggest taking a look there from time to time.

## How to use NRun? 📚

Using NRun is very simple. If you want to create a new service just add shell script (with [shebang](https://en.wikipedia.org/wiki/Shebang_(Unix)) at top of the file), link to executable or executable file you want to run each time. Make sure every file in `/var/on` has execute permissions by running: `chmod +x /var/on/*`. If file wont be executable it wont run.

Example command to create symlink to executable `echo`:
```
sudo ln -s /usr/bin/echo /var/on
```

If you want to shutdown or reboot you can use utils in `utils/` directory. Compile these:
```
mkdir bin
gcc utils/poweroff.c -o bin/poweroff
gcc utils/reboot.c -o bin/reboot
```

These executables work by sending SIGTERM (shutting down) or SIGINT (rebooting) to PID 1. You dont have to use these utils because you can just simply do that with `kill` command:

```
kill -s SIGTERM 1 # shutting down
kill -s SIGINT 1 # rebooting
```

## How to uninstall NRun? ❌

If your OS properly with NRun installation is very easy. Just edit `/etc/default/grub` and remove `init=/usr/bin/nrun` from `GRUB_CMDLINE_LINUX_DEFAULT` line.
However, if your OS does not boot properly for some reason, create a bootable USB with any Linux distribution and plug your USB to your PC. Enter your BIOS and set your USB as boot option, then save and exit. In your Linux distro mount your partitions.

For example, if your partitions look like this:
```
├─sdb1   8:17   0     1G  0 part /boot/efi
├─sdb2   8:18   0    50G  0 part /
├─sdb3   8:19   0   330G  0 part /home
```

Run following commands:
```
sudo mount /dev/sdb2 /mnt
sudo mount /dev/sdb1 /mnt/boot/efi
sudo mount /dev/sdb3 /mnt/home
```

Of course this is just a example. To view your partitions run `lsblk` command.

Then, use `chroot` (or `xchroot`/`arch-chroot` if running Void/Arch Linux) and edit `/etc/default/grub`:
```
chroot /mnt
EDITOR="your_favourite_editor"
$EDITOR /etc/default/grub
```

Find `GRUB_CMDLINE_LINUX_DEFAULT` line and remove `init=/usr/bin/nrun` from there. Then just run:
```
exit
umount -a
sudo reboot
```

And boot into your OS, you should use your old init, unless you removed it, if you did I recommend reinstalling your OS so you wont have any problems in the future.

## Contact & Links 📧

Discord: `@zerfithel`
Website: [zerfithel.github.io](https://zerfithel.github.io/)
