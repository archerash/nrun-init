#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "include/poweroff.h"
#include "include/msg.h"

#define POWER_OFF 1
#define REBOOT 2

// shutdown
static void sigterm_handler(int signum) {
  pid_t pid = fork();
  if (pid == 0) {
    execl("/etc/re/3", "/etc/re/3", (char *)NULL);
  }
  re_stop(POWER_OFF);
}

// reboot
static void sigint_handler(int signum) {
  pid_t pid = fork();
  if (pid == 0) {
    execl("/etc/re/3", "/etc/re/3", (char *)NULL);
  }
  re_stop(REBOOT);
}

// clean up zombie processes
static void sigchld_handler(int signum) {
  pid_t pid;
  int status;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    printf("Cleaned up process: %d\n", pid);
  }
}

// daemon
void daemon_mode(void) {
  signal(SIGTERM, sigterm_handler); // if SIGTERM then shutdown
  signal(SIGINT, sigint_handler); // if SIGINT then reboot
  signal(SIGCHLD, sigchld_handler); // if SIGCHLD then clean up zombie
  while (1) {
    pause(); // waits for signal
  }
}
