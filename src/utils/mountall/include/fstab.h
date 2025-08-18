#ifndef _FSTAB_H
#define _FSTAB_H
#include <stddef.h>
struct fstab {
  char *uuid; // fs_spec
  char *mnt; // fs_file
  char *fstype; // fs_vfstype
  char *mntopts; // fs_mntops
  int fs_dump; // fs_dump
  int fs_pass; // fs_pass
};
int readfstab(int fd, int line, struct fstab *fs);
#endif
