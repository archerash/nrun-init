#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <signal.h>

// poweroff
void sigterm_handler(int signum) {
  pid_t pid = fork();
  // error
  if (pid < 0) {
    fprintf(stderr, "Failed to fork process to shutdown: %s\n", strerror(errno));
    _exit(1);
  }

  if (pid == 0) {
    // execute /etc/re/3 (child)
    execl("/etc/re/3", "/etc/re/3", (char *)NULL);
    _exit(1);
  } else {
    // parent
    int status;
    waitpid(pid, &status, 0); // wait for child to end

    // shutdown
    reboot(RB_POWER_OFF);
  }
}

// reboot
void sigint_handler(int signum) {
  pid_t pid = fork();
  // error
  if (pid < 0) {
    fprintf(stderr, "Failed to fork process to reboot: %s\n", strerror(errno));
    _exit(1);
  }

  if (pid == 0) {
    // execute /etc/re/3 (child)
    execl("/etc/re/3", "/etc/re/3", (char *)NULL);
    _exit(1);
  } else {
    // parent
    int status;
    waitpid(pid, &status, 0); // wait for child to end
    
    // reboot
    reboot(RB_AUTOBOOT);
  }
}

// clean up zombie processes
void sigchld_handler(int signum) {
  pid_t pid;
  int status;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    printf("Cleaned up process: %d\n", pid);
  }
}

