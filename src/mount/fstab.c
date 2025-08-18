#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "include/fstab.h"

int readfstab(int fd, int line, struct fstab *fs) {
  // Input validation
  if (fd < 0 || line < 1 || fs == NULL) return -1;

  // Get file information
  struct stat st;
  if (fstat(fd, &st) == -1) return -1;
  if (st.st_size < 1) return -1;

  // Allocate memory for file contents
  char *buffer = malloc(st.st_size + 1);
  if (!buffer) return -1;

  // Read fstab content
  lseek(fd, 0, SEEK_SET);
  ssize_t n = read(fd, buffer, st.st_size);
  if (n != st.st_size) {
    free(buffer);
    return -1;
  }
  buffer[st.st_size] = '\0';

  // Find requested line in buffer
  char *start = buffer;
  char *end;
  int current_line = 1;
  while (current_line < line && (end = strchr(start, '\n')) != NULL) {
    start = end + 1;
    current_line++;
  }

  // If line does not exist return 1
  if (current_line != line) {
    free(buffer);
    return 1;
  }
  
  // Terminate the line at newline char
  end = strchr(start, '\n');
  if (end) *end = '\0';

  // Skip comments and empty lines
  if (start[0] == '#' || start[0] == '\0') {
    free(buffer);
    return 1;
  }

  // Add data to structure
  char *token;
  int field = 0;
  token = strtok(start, " \t"); // Cut /etc/fstab on tokens
  while (token) {
    if (field == 0) fs->uuid = strdup(token); // UUID
    else if (field == 1) fs->mnt = strdup(token); // Mountpoint
    else if (field == 2) fs->fstype = strdup(token); // Filesystem
    else if (field == 3) fs->mntopts = strdup(token); // Mount opts
    else if (field == 4) fs->fs_dump = atoi(token); // dump
    else if (field == 5) fs->fs_pass = atoi(token); // pass
    field++;
    token = strtok(NULL, " \t"); // Get next token
  }

  free(buffer);
  return 0;
}
