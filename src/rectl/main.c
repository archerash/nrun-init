#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/resv.h"
#include "include/rerun.h"
#include "include/resig.h"

#define TERM 15
#define INT 2

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s [OPTION] <arguments>...\n"
            "Available options:\n"
            "\n"
            "svdir\t Runs all services in given directory\n"
            "sv\t Runs single service and monitors it\n"
            "\n"
            "Full documentation: <https://zerfithel.github.io/software/re/rectl\n"
            "Author: Zerfithel\n", argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "svdir") == 0) rerun(argv[2]);
  else if (strcmp(argv[1], "sv") == 0) resv(atoi(argv[2]), atoi(argv[3]), argv[4]);
  else if (strcmp(argv[1], "poweroff") == 0) resig(TERM);
  else if (strcmp(argv[1], "reboot") == 0) resig(INT);

  return 0;
}
