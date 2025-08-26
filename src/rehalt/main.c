#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "include/pkill.h"

int main(void) {
  if (rehalt_pkill() != 0) {
    fprintf(stderr, "Failed to halt system (stage 1: pkill): %s\n", strerror(errno));
    return 1;
  }

  // i will add umount in next updates

  return 0;

}
