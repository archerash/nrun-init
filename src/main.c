#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/reboot.h>
#include <sys/wait.h>

#include "include/mount.h" // stage 0
#include "include/services.h" // stage 1
#include "include/daemon.h" // stage 2
#include "include/msg.h" // errmsg

int main(void) {
  if (getpid() > 1) {
    errmsg("re should be ran as PID 1\n");
    return 1;
  }

  // Running udevd
  if (fork() == 0) {
    if (execl("/sbin/udevd", "udevd", "--daemon", NULL) == -1) {
      errmsg("Failed to run udevd: %s\n", strerror(errno));
      _exit(1);
    }
  }

  // Stage 0: Mounting devices: mount.c
  printf("=> re: Stage 0: Mounting devices\n");
  if (stage0() != 0) {
    fprintf(stderr, "\n");
  }

  // Stage 1: Running services: services.c
  printf("=> re: Stage 1: Running services\n");
  stage1();

  // Stage 2: Entering daemon mode: daemon.c
  printf("=> re: Stage 2: Entering daemon mode\n");
  start_daemon();

  // This block should never be reached
  errmsg("re has reached block of code that should never be reached... quitting.");
  sync();
  reboot(RB_POWER_OFF);
  return 1;
}

