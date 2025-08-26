#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/reboot.h>
#include <sys/wait.h>

#include "include/handler.h"

int main(void) {
  // Ensure the process is running as PID 1
  if (getpid() > 1) {
    fprintf(stderr, "re should be run as PID 1\n");
    return 1;
  }

  // Start stages /etc/re/1 and /etc/re/2 sequentially
  for (int i = 1; i <= 2; i++) {
    pid_t pid = fork();
    if (pid < 0) {
      fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
      exit(1);
    }

    if (pid == 0) { // Child process
      char *path;
      if (asprintf(&path, "/etc/re/%d", i) == -1) {
        fprintf(stderr, "Failed to create path for stage %d: %s\n", i, strerror(errno));
        _exit(1);
      }

      execl(path, path, (char *)NULL); // Replace child with the stage process

      // If execl fails
      fprintf(stderr, "Failed to run %s: %s\n", path, strerror(errno));
      free(path);
      _exit(1); // Child exit
    } else { // Parent process
      int status;
      if (waitpid(pid, &status, 0) < 0) { // Wait for child to finish
        fprintf(stderr, "Error waiting for child: %s\n", strerror(errno));
      }
    }
  }

  struct sigaction saction;

  // Setup SIGTERM handler for shutdown
  memset(&saction, 0, sizeof(saction));
  saction.sa_handler = sigterm_handler;
  sigemptyset(&saction.sa_mask);
  saction.sa_flags = SA_RESTART;
  if (sigaction(SIGTERM, &saction, NULL) < 0) {
    fprintf(stderr, "sigaction for SIGTERM failed: %s\n", strerror(errno));
  }

  // Setup SIGINT handler for reboot
  memset(&saction, 0, sizeof(saction));
  saction.sa_handler = sigint_handler;
  sigemptyset(&saction.sa_mask);
  saction.sa_flags = SA_RESTART;
  if (sigaction(SIGINT, &saction, NULL) < 0) {
    fprintf(stderr, "sigaction for SIGINT failed: %s\n", strerror(errno));
  }

  // Setup SIGCHLD handler to clean up finished child processes
  memset(&saction, 0, sizeof(saction));
  saction.sa_handler = sigchld_handler;
  sigemptyset(&saction.sa_mask);
  saction.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  if (sigaction(SIGCHLD, &saction, NULL) < 0) {
    fprintf(stderr, "sigaction for SIGCHLD failed: %s\n", strerror(errno));
  }

  // Main loop
  while (1) {
    pause(); // Wait for any signal
  }

  // This code should never be reached
  fprintf(stderr, "=> REACHED BLOCK OF CODE THAT SHOULD NEVER BE REACHED... SHUTTING DOWN\n");
  sync();
  reboot(RB_POWER_OFF);

  return 0;
}
