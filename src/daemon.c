#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "include/poweroff.h"

// shutdown
static void sigterm_handler(int signum) {
  printf("=> nrun: entering stage 3...\npowering off...\n");
  nrun_poweroff();
}

// reboot
static void sigint_handler(int signum) {
  printf("=> nrun: entering stage 3...\nrebooting...\n");
  nrun_reboot();
}

// clean up zombie processes
static void sigchld_handler(int signum) {
  pid_t pid;
  int status;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    printf("cleaned up finished process: %d\n", pid);
  }
}

// daemon
void start_daemon(void) {
  signal(SIGTERM, sigterm_handler); // if SIGTERM then shutdown
  signal(SIGINT, sigint_handler); // if SIGINT then reboot
  signal(SIGCHLD, sigchld_handler); // if SIGCHLD then clean up zombie
  while (1) {
    pause(); // waits for signal
  }
}
