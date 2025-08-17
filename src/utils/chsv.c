#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define UTIL_NAME "chsv"

int main(int argc, char **argv) {
  if (argc == 1) {
  help:
    printf("Usage: %s [OPTION] <Service>\n"
           "Available options:\n"
           "\n"
           "start\tStarts given service\n"
           "stop\tStops given service\n"
           "refresh\tRestarts all services from /var/on\n"
           "add\tAdds given service to /var/on\n"
           "remove\tRemoves given service from /var/on\n"
           "help\tShows this message\n"
           "\n"
           "This software is part on \"Re\" init system. Full documentation: <https://zerfithel.github.io/software/chsv>\n", UTIL_NAME);
    return 1;
  } else if (argc == 2 && strcmp(argv[1], "--help") == 0) {
    goto help;
  }

  
}
