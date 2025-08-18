#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/reboot.h>
#include <sys/wait.h>

#include "include/services.h"
#include "include/daemon.h"
#include "include/msg.h"

int main(void) {
  if (getpid() > 1) {
    errmsg("re should be ran as PID 1\n");
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


  daemon_mode();

  // Code of block that should never be reached
  fprintf(stderr, "=> REACHED BLOCK OF CODE THAT SHOULD NEVER BE REACHED... SHUTTING DOWN\n");
  sync();
  reboot(RB_POWER_OFF);

  return 0;
}
