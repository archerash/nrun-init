#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <limits.h>

#include "include/vector.h"

#define MTAB_PATH "/etc/mtab" // Path to MTAB file
#define BUFFER_SIZE PATH_MAX+1024 // This defines max safe line lenght (it should be safe for most systems)

// Function to compare length (used in qsort())
int cmp_len(const void *a, const void *b) {
  const char *s1 = *(const char **)a;
  const char *s2 = *(const char **)b;
  return strlen(s2) - strlen(s1);
}

// Function to unmount all mounted filesystems by descending order
int rehalt_unmount(void) {
  /* Reading /etc/mtab file and saving mounted mountpoints to vector */

  FILE *fp = fopen(MTAB_PATH, "r"); // Open /etc/mtab
  if (fp == NULL) {
    // Error handle
    fprintf(stderr, "Failed to open file: %s: %s\n", MTAB_PATH, strerror(errno));
    return 1;
  }

  Vector *mountpoints = create_vector(); // Create vector to hold mountpoints

  char buffer[BUFFER_SIZE]; // Buffer to temporarily hold lines content

  while (fgets(buffer, sizeof(buffer), fp)) {
    char *path = strtok(buffer, " \t"); // Tokenize the line with " " delimiter
    path = strtok(NULL, " \t"); // Get second element of line
    push_back_string(mountpoints, path); // Push that element to "mountpoints" vector
  }

  fclose(fp); // Close file
  
  /* Sort the vector in descending order by the length of the mountpoints */

  qsort(mountpoints->data, mountpoints->size, sizeof (char *), cmp_len); // Sort vector

  // Unmounting filesystems
  for (int i = 0; i < vector_size(mountpoints); i++) {
    if (umount(vector_at(mountpoints, i)) != 0) {
      // Error handle
      fprintf(stderr, "Failed to unmount: %s: %s\n", vector_at(mountpoints, i));
      continue; // Skip this mountpoint
    }
  }

  return 0;
}
