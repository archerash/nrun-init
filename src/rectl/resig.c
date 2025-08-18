#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#define TERM 15
#define INT 2

int resig(const int SIGNAL) {
  if (kill(1, SIGNAL) != 0) {
    if (SIGNAL == TERM) {
      fprintf(stderr, "Failed to shutdown: %s\n", strerror(errno));
    } else {
      fprintf(stderr, "Failed to reboot: %s\n", strerror(errno));
    }
    return 1;
  }
  return 0;
}
