#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>

#define PROC_DIR "/proc"

int main(void) {
  struct dirent *entry;
  DIR *proc = opendir(PROC_DIR);
  if (proc == NULL) return 1;

  pid_t halt_pid = getpid();
  chdir(PROC_DIR);

  // Iterate on every file inside /proc
  while ((entry = readdir(proc)) != NULL) {
    // Skip both . and ..
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

    // Structure to save data about file
    struct stat st;
    // Stat data about file to structure
    if (lstat(entry->d_name, &st) != 0) {
      fprintf(stderr, "Failed to stat file: %s: %s\n", entry->d_name, strerror(errno));
      continue;
    }
  
    // If its regular file continue
    if (S_ISREG(st.st_mode)) continue;
    
    // If its directory
    if (S_ISDIR(st.st_mode)) {
      long pid = atoi(entry->d_name); // Convert dir name to long

      if (pid == 0) continue; // Skip kernel PID
      if (pid == 1) continue; // Skip init system PID
      if (pid == halt_pid) continue; // If found process is equal to halt then skip

      kill((pid_t)pid, SIGKILL); // Send SIGKILL to process
    }
  }
  kill(halt_pid, SIGKILL); // Kill itself
  closedir(proc);
  return 0;
}
