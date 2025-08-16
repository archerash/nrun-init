#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

int stage1(void) {
  struct dirent *entry;

  DIR *dir = opendir("/var/on");
  printf("opened /var/on\n");
  if (dir == NULL) {
    fprintf(stderr, "An error occured while trying to open /var/on directory: %s\n", strerror(errno));
    exit (1);
  }

  chdir("/var/on");
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
    

    pid_t pid = fork();
    if (pid == 0) {
      // Child process
      printf("running service: %s\n", entry->d_name);
      execl(entry->d_name, (char *)NULL);
      perror("execl");
      _exit(1);
    } else if (pid < 0) {
      perror("fork");
    }
    // Parent process goes over to the next service file
}

  return 0;
}
