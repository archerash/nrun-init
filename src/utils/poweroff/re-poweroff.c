#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

/* Shutting down system with re is based on sending SIGTERM (Signal Terminate) to PID 1. 
However, this may require root. You can use `kill -s SIGTERM 1` instead of this command 
but I suggest compiling it and adding it to /usr/bin to invoke it shortly. You can name
it re-shutdown to not remove your old shutdown command for old init. This poweroff command
might work for other inits like runit but this is not guaranteed. If you want to, please
leave this repository a star. I am working hard on it. Thank you for reading its source :> */

int main(int argc, char **argv) {
  if (argc == 1) {
    // If no time was specified, just shutdown
    goto shutdown;
  } else {
    // Sleep for given amount of time and shutdown
    sleep(atoi(argv[2]));
    goto shutdown;
  }

shutdown:
  if (kill(1, SIGTERM) != 0) {
    fprintf(stderr, "shutdown: Failed to send SIGTERM to PID 1: %s\n", strerror(errno));
    return 1;
  }
  return 0;
}
