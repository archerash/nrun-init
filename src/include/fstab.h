#ifndef _FSTAB_H
#define _FSTAB_H
#include <stddef.h>
#include "struct.h"
int readfstab(int fd, int line, struct fstab *fs);
#endif
