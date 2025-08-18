#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

int rerun(const char *path) {
  struct dirent *entry;

  // Open service dir
  DIR *dir = opendir(path);
  if (dir == NULL) {
    fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
    return 1;
  }

  printf("-> Opened %s\n", path);

  // Execute all scripts in given directory
  chdir(path);
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

    pid_t pid = fork();
    if (pid == 0) {
      // Child process
      printf("Running service: %s\n", entry->d_name);
      execl(entry->d_name, (char *)NULL);
      perror("execl");
      _exit(1);
    } else if (pid < 0) {
      fprintf(stderr, "Failed to create new process: %s\n", strerror(errno));
    }
    // Parent process goes over to the next service file
  }

  return 0;
}

