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
  if (getpid() > 1) {
    fprintf(stderr, "re should be ran as PID 1\n");
    return 1;
  }
  
  for (int i = 1; i <= 2; i++) {
    pid_t pid = fork();

    if (pid < 0) {
      fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
      exit(1);
    }

    // Child
    if (pid == 0) {
      char *path;
      if (asprintf(&path, "/etc/re/%d", i) == -1) {
        fprintf(stderr, "Failed to create path for stage %d: %s\n", i, strerror(errno));
        return 1;
      }
        
      if (execl(path, path, (char *)NULL) != 0) {
        fprintf(stderr, "Failed to run /etc/re/%d: %s\n", i, strerror(errno));
        free(path);
        exit(1);
      }

      free(path);
    } else { // parent
      int status;
      waitpid(pid, &status, 0); // Waiting for child to end
    }
  }

  struct sigaction saction;

  // SIGTERM => Shutdown
  memset(&saction, 0, sizeof(saction));
  saction.sa_handler = sigterm_handler;
  sigemptyset(&saction.sa_mask);
  saction.sa_flags = SA_RESTART;
  sigaction(SIGTERM, &saction, NULL);

  // SIGINT => Reboot
  memset(&saction, 0, sizeof(saction));
  saction.sa_handler = sigint_handler;
  sigemptyset(&saction.sa_mask);
  saction.sa_flags = SA_RESTART;
  sigaction(SIGINT, &saction, NULL);

  // SIGCHLD => Clean up finished child
  memset(&saction, 0, sizeof(saction));
  saction.sa_handler = sigchld_handler;
  sigemptyset(&saction.sa_mask);
  saction.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &saction, NULL);

  while (1) {
    pause(); // waits for signal
  }

  // Code of block that should never be reached
  fprintf(stderr, "=> REACHED BLOCK OF CODE THAT SHOULD NEVER BE REACHED... SHUTTING DOWN\n");
  sync();
  reboot(RB_POWER_OFF);

  return 0;
}
