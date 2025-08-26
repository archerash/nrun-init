# RE - Lightweight and minimalist init system for Linux systems

RE is a simple and lightweight init system designed for Linux systems. It is supposed to be ran as PID 1 (init) for it to work.

## How does RE work?

When the bootloader loads the kernel into RAM, it may also load the initramfs (a temporary filesystem). The kernel unpacks the initramfs and uses it to mount the root filesystem (your root disk partition). Then it runs init, which becomes the first process (PID 1) in Unix systems like Debian or Arch. That is exactly what RE is, it is that "init" file. RE is very simple and it works by running "stages scripts" that can be found in `/etc/re` directory. First, at boot, RE starts `/etc/re/1` shell script. When this script returns SIGCHLD (it ends) it runs `/etc/re/2` and after it returns SIGCHLD (it ends) RE enters "daemon mode" in which it waits for signals. If it receives `SIGCHLD` (signal that indicates a child process has ended) it cleans up finished process. If it receives `SIGTERM` or `SIGINT` it runs `/etc/re/3` and `shutdowns` if it was SIGTERM and `reboots` if it was SIGINT.

## What are stages scripts?

Stages scripts are just shell scripts that are ran by the init during boot (1, 2) or shutdown (3). The first stage script in default RE configuration executes `rectl` that runs all services from `/etc/re/core-services` directory. Services there are core services that need to be ran for system to work or other essential services. Second stage script runs `rectl` that runs all services in `/var/sv` directory. You should add most services to `/var/sv` directory (ex. lightdm, agetty, dhcpcd). The last script (third one) in default RE configuration runs `rehalt` which sends `SIGKILL` signals to all processes except kernel and init system. When all processes are dead, it unmounts all devices from `/etc/mtab` and it kills itself. You can edit these stages scripts with your text editor to change how your system boots/shutdowns. You should NOT add commands to shutdown/reboot OS in `/etc/re/3`.

## How to use RE?

Using RE is very simple. Here is a simple "graph" of directories of RE and its use.
```
/etc/re -> RE root directory
├── 1 -> Stage script 1
├── 2 -> Stage script 2
├── 3 -> Stage script 3
├── core-services -> Directory for core services
│   ├── filesystem 
│   ├── hostname
│   ├── modules
│   └── udevd
└── sv -> Directory for normal services
    ├── agetty-tty1
    ├── audio
    ├── udevd
    ├── lightdm
    └── dhcpcd
```

Now you can see that services are located in `/etc/re/sv`, but by default these are not ran by RE. To enable these you can either edit your `/etc/re/2` script to run `rectl` on `/etc/re/sv` directory but this is not recommended as it runs all scripts from `/var/sv`. It is recommended to create symlinks in `/var/sv` directory to all services from `/etc/re/sv` you want enabled (ex. to enable all services from /etc/re/sv run: `ln -s /etc/re/sv/* /var/sv` - this command will create symlinks in `/var/sv` directory to all files in `/etc/re/sv` directory). To disable a service simply run `rm /var/sv/<service>`. RE also comes with other handy tools like `rehalt` which can be used to halt system. Here is a small "graph" with utils and its use:
```
rehalt -> Used to halt system
rectl -> Can be used to <shutdown/reboot> or <run service directory/run single service and monitor it>
mountall -> Used to mount all devices from /etc/fstab in given mountpoints with given flags
```

While these utils are not required for system to work it is recommended to use them (especially the `rehalt` and `rectl`). I suggest reading small RE wiki about rectl to understand how it works and how to use it [here](https://zerfithel.github.io/software/re/rectl). You can also read man page in this repo inside `man/` directory.

## How to install RE?

It is recommended to have `POSIX Shell` installed to use RE to not use binary files to do simple things (so your stage scripts can be shell scripts, not binaries). This guide assumes you are using `GRUB` as your bootloader.

Build dependencies:
`C Compiler (gcc)`
`git`
`(recommended) make`
`(recommended) bash`

To quickly setup RE on your system you can run `INSTALL.sh` file in this repository, if you dont want to, here is manual guide:

1. Cloning this repository (if not cloned already):
```
cd $HOME/
git clone https://github.com/zerfithel/re
cd re
```

2. Compiling
```
mkdir bin/
make
```

- If you do not have make installed run following commands instead:
```
gcc src/init/*.c src/init/include/*.h -o bin/re
gcc src/mountall/*.c src/mountall/include/*.h -o bin/mountall
gcc src/rectl/*.c src/rectl/include/*.h -o bin/rectl
gcc src/rehalt*.c src/include/*.h -o bin/rehalt
```

3. Creating RE directories
```
sudo mkdir -p /etc/re
sudo mkdir -p /etc/re/core-services
sudo mkdir -p /etc/re/sv
sudo mkdir /var/sv
```

4. Adding basic services and stage scripts
- Clone `re-services` repository:
```
git clone https://github.com/zerfithel/re-services
sudo cp re-services/sv/basic/* /etc/re/sv
sudo chmod +x /etc/re/sv/*
sudo ln -s /etc/re/sv/* /var/sv

sudo cp re-services/core-services/basic/* /etc/re/core-services
sudo chmod +x /etc/re/core-services/*

sudo cp re-services/stages/basic/{1,2,3} /etc/re
sudo chmod +x /etc/re/{1,2,3}
```

- You can add more services if you want like: `lightdm`, `network`. These are just basic services for you to test RE.

5. Moving binaries in desired directories
```
sudo chmod +x bin/*
sudo mv bin/* /usr/bin
```

6. Rebooting and testing

Now, lets reboot and test if RE is working properly. Run following command
```
sudo reboot
```

In GRUB click `e` while targeting label to boot your OS. Find line that begins with `linux` and add `init=/usr/bin/re` at the end of it. It is recommended to also have `ro` parameter (to enable read-only mode, it will be changed to read-write after fsck scan if it wont return any errors).

Then press `CTRL-X` and it will start your OS with your new `CMDLINE` parameters. When your OS boots login and check if your filesystems are mounted (ex. if you have separate /home partition check if you have files in `/home` by running `ls /home` and do same for all partitions). If everything is working, you can make RE your default init.

7. Making RE default init
- Reboot system
```
sudo rectl reboot
```

- Enter your OS with your old init, login, and edit `/etc/default/grub`, find `GRUB_CMDLINE_LINUX_DEFAULT` line and add `init=/usr/bin/re` at the end of it.

- Rebuild your GRUB configuration
```
sudo update-grub || sudo grub-mkconfig -o /boot/grub/grub.cfg
```

- Reboot
```
sudo reboot
```

8. (Optional & Not Recommended) Uninstalling old init

If you wish to uninstall your old init (which is not recommended in case you wanting to come back to your old init) please refer to your distro wiki.

## How to use my old init again and uninstall RE?

- If your system boots with RE:

1. Edit `/etc/default/grub` and remove `init=/usr/bin/re` and anything you added there to install RE.
2. Update your grub configuration:
```
sudo update-grub || sudo grub-mkconfig -o /boot/grub/grub.cfg
```
3. Reboot:
```
sudo rectl reboot
```

- If your system fails to boot with RE:

1. Create bootable USB with Ventoy/BalenaEtcher or any other tool to create bootable USBs (with Linux installed there).
2. Plug in your USB, enter BIOS, select your USB as boot option #1.
3. Mount your partitions inside live boot ISO by:

- To check your partitions:
```
lsblk
```

Find your system partitions there and mount root (/) partition first, then the other ones. For example if your partitions look like this:
```
├─sda1   8:1    0     1G  0 part /boot/efi
├─sda2   8:2    0    50G  0 part /
├─sda3   8:3    0   330G  0 part /home
```
Then run following commands:

```
sudo mkdir /mnt
sudo mount /dev/sda2 /mnt
sudo mount /dev/sda1 /mnt/boot/efi
sudo mount /dev/sdb3 /mnt/home
```

It is important to mount root first and then partitions that mountpoints are closer to root (/).

4. Change root into your mounted filesystem:
```
sudo chroot /mnt
```

5. Edit your `/etc/default/grub` by removing `init=/usr/bin/re` and any other parameters you added there to make RE work.
6. Rebuild your GRUB configuration:
```
sudo grub-update || sudo grub-mkconfig -o /boot/grub/grub.cfg
```

7. Reboot, wait, unplug USB and boot into your OS.

If you uninstalled your old init you will have to install it again, please refer to your distro wiki. If you did though I highly recommend reinstalling your OS to avoid weird errors.

8. (Optional) Uninstalling RE:
```
sudo rm -rf /etc/re
sudo rm -rf /var/sv
sudo rm -f /usr/bin/re /usr/bin/rectl /usr/bin/rehalt /usr/bin/mountall
```

## Contact & Links

Website: [zerfithel.github.io](https://zerfithel.github.io)

See RE wiki: [software/re](https://zerfithel.github.io/software/re)

Learn how to configure RE: [re/configuration](https://zerfithel.github.io/software/re/configuration)

Check out re-services repository: [zerfithel/re-services](https://github.com/zerfithel/re-services)

Discord: `@zerfithel`
