#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <signal.h>

#include "include/sig_atomic_t.h"

volatile sig_atomic_t shutdown_r = 0;
volatile sig_atomic_t reboot_r = 0;

// Handler SIGTERM (poweroff)
void sigterm_handler(int signum) {
  shutdown_r = 1;
}

// Handler SIGINT (reboot)
void sigint_handler(int signum) {
  reboot_r = 1;
}

// Handler SIGCHLD (child reaping)
void sigchld_handler(int signum) {
  pid_t pid;
  int status;
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    // reap child
  }
}
