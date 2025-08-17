#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <sys/reboot.h>
#include <errno.h>

#include "include/msg.h"

void re_stop(const int SIGNAL) {
  // Send SIGTERM (or SIGKILL if SIGKILL wont work) to all processes except PID 1
  DIR *proc = opendir("/proc");
  if (proc == NULL) {
    errmsg("Failed to open /proc: %s\n", strerror(errno));
  }

  struct dirent *entry;
  while ((entry = readdir(proc)) != NULL) {
    char *endptr;
    long pid = strtol(entry->d_name, &endptr, 10);
    /* Skip both PID 1 and folders that
    are not numbers */

    if (*endptr != '\0') continue;
    if (pid == 1) continue;

    kill((pid_t)pid, SIGKILL); // Send SIGKILL to all processes except PID 1 (nrun)
  }
  closedir(proc);
  
  // Synchronise and do shutdown, reboot or sleep
  sync();
  
  if (SIGNAL == 1) {
    reboot(RB_POWER_OFF);
  } else if (SIGNAL == 2) {
    reboot(RB_AUTOBOOT);
  } else {
    reboot(RB_POWER_OFF);
  }

}

