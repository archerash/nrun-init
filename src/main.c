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

#define CORE_SERVICES_DIR "/etc/re/core-services"
#define SERVICES_DIR "/var/sv"

int main(void) {
  if (getpid() > 1) {
    errmsg("re should be ran as PID 1\n");
    return 1;
  }

  // Stage 1: Running core services
  printf("=> re: Stage 1: Running core services\n");
  if (run_services(CORE_SERVICES_DIR) != 0) {
    errmsg("--> Failed to run core services\n");
  }

  // Stage 2: Running post-mount services: services.c
  printf("=> re: Stage 2: Running post-mount services\n");
  if (run_services(SERVICES_DIR) != 0) {
    errmsg("--> Failed to run post-mount services\n");
  }

  // Stage 3: Entering daemon mode: daemon.c
  printf("=> re: Stage 3: Entering daemon mode\n");
  daemon_mode();

  // This block should never be reached
  errmsg("--> re has reached block of code that should never be reached... quitting.");
  sync();
  reboot(RB_POWER_OFF);
  return 1;
}

