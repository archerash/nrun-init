#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    goto shutdown;
  } else {
    sleep(atoi(argv[2]));
    goto shutdown;
  }

shutdown:
  if (kill(1, SIGINT) != 0) {
    fprintf(stderr, "reboot: Failed to send SIGINT to PID 1: %s\n", strerror(errno));
    return 1;
  }
  return 0;
}

