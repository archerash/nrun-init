#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

/* Rebooting system with re is based on sending SIGINT (Signal Interrupt) to PID 1. 
However, this may require root. You can use `kill -s SIGINT 1` instead of this command 
but I suggest compiling it and adding it to /usr/bin to invoke it shortly. You can name
it re-reboot to not remove your old reboot command for old init. This reboot command
might work for other inits like runit but this is not guaranteed. If you want to, please
leave this repository a star. I am working hard on it. Thank you for reading its source :> */

int main(int argc, char **argv) {
  if (argc == 1) {
    // If not time was specified go to shutdown
    goto shutdown;
  } else {
    // If time was specified, wait for given amount of time and then shutdown
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

