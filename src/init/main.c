/* ==========================================================================
   GitHub Page & Full Source Code: <https://github.com/zerfithel/re>
   Author: @zerfithel <https://github.com/zerfithel> 
   
   THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
   APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
   HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY
   OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM
   IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
   ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 
   ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/reboot.h>
#include <sys/wait.h>

#include "include/handler.h"
#include "include/sig_atomic_t.h"

#define STAGE_1 "/etc/re/1"
#define STAGE_2 "/etc/re/2"
#define STAGE_3 "/etc/re/3"

void set_signal(int sig_, void (*handler)(int), int flags) {
  struct sigaction saction;
  memset(&saction, 0, sizeof(saction));
  saction.sa_handler = handler;
  sigemptyset(&saction.sa_mask);
  saction.sa_flags = flags;
  if (sigaction(sig_, &saction, NULL) < 0) {
    fprintf(stderr, "Failed to set sigaction for signal: %d: %s\n", sig_, strerror(errno));
  }
}

int main(void) {
  // Ensure the process is running as PID 1
  if (getpid() > 1) {
    fprintf(stderr, "re should be run as PID 1\n");
    return 1;
  }

  // Setup sigaction
  set_signal(SIGTERM, sigterm_handler, 0);
  set_signal(SIGINT, sigint_handler, 0);
  set_signal(SIGCHLD, sigchld_handler, SA_RESTART | SA_NOCLDSTOP);

  {
    pid_t pid;

    // STAGE 1
    pid = fork(); // Fork process
    if (pid < 0) {
      fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
      _exit(1);
    }
  
    if (pid == 0) {
      // Child
      // Replace child process with stage 1 script
      execl(STAGE_1, STAGE_1, (char *)NULL);
      fprintf(stderr, "Failed to run: %s: %s\n", STAGE_1, strerror(errno));
      _exit(1);
    } else if (pid > 0) {
      // Parent
      waitpid(pid, NULL, 0); // Wait for child to finish
    }

    // STAGE 2
    pid = fork(); // Fork process
    if (pid < 0) {
      fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
      _exit(1);
    }

    if (pid == 0) {
      // Child
      // Replace child process with stage 2 script
      execl(STAGE_2, STAGE_2, (char *)NULL);
      fprintf(stderr, "Failed to run: %s: %s\n", STAGE_2, strerror(errno));
      _exit(1);
    } else if (pid > 0) {
      // Parent
      waitpid(pid, NULL, 0);
    }
  }

  // Main loop
  while (1) {
    if (shutdown_r == 1) {
      pid_t pid = fork();
      // If fork failed
      if (pid < 0) {
        fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
        _exit(1);
      }

      if (pid == 0) {
        execl(STAGE_3, STAGE_3, (char *)NULL); // Execute stage 3 script
        _exit(1); // If execl will fail
      } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0); // Wait for child
        reboot(RB_POWER_OFF); // Power off
      }
      shutdown_r = 0; // Flag reset
    }

    if (reboot_r == 1) {
      pid_t pid = fork();
      // If fork failed
      if (pid < 0) {
        fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
        _exit(1);
      }

      if (pid == 0) {
        execl(STAGE_3, STAGE_3, (char *)NULL); // Execute stage 3 script
        _exit(1); // If execl will fail
      } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0); // wait for child
        reboot(RB_AUTOBOOT); // Reboot
      }
      shutdown_r = 0; // Flag reset
    }
    pause(); // Wait for signals
  }

  // This code should never be reached
  fprintf(stderr, "=> RE reached block of code that should never be reached, shutting down...\n");
  sync();
  reboot(RB_POWER_OFF);

  return -1;
}
