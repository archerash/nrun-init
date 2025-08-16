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

int main(void) {
  if (getpid() > 1) {
    fprintf(stderr, "nrun should be ran as PID 1!\n");
    return 1;
  }
  
  // Stage 0: Mounting devices: mount.c
  printf("=> nrun: entering stage 0\nMounting devices\n");
  if (stage0() != 0) {
    fprintf(stderr, "Stage 0 did not return EXIT_SUCCESS...\n");
  }

  // Stage 1: Running services: services.c
  printf("=> nrun: entering stage 1\nRunning services\n");
  stage1();

  // Stage 2: Entering daemon mode: daemon.c
  printf("=> nrun: entering stage 2\nEntering daemon mode\n");
  start_daemon();

  // This block should never be reached
  fprintf(stderr, "nrun has reached block of code that should never be reached... quitting...\n");
  sync();
  reboot(RB_POWER_OFF);
  return 1;
}

