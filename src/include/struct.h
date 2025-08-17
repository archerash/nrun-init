#ifndef _STRUCT_H
#define _STRUCT_H
#include <stddef.h>
/* Comments indicate what is alternative name in /usr/include/fstab.h */
struct fstab {
  char *uuid; // fs_spec
  char *mnt; // fs_file
  char *fstype; // fs_vfstype
  char *mntopts; // fs_mntops
  int fs_dump; // fs_dump
  int fs_pass; // fs_pass
};
#endif
