#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include "include/pkill.h"
#include "include/unmount.h"

#define UTIL_NAME "rehalt"

int main(void) {

  {
    // Forking process
    pid_t pid = fork();

    // Error
    if (pid < 0) goto fork_fail;

    if (pid == 0) {
      // Child
      if (rehalt_pkill() != 0) {
        fprintf(stderr, "Failed to halt system: stage 1: pkill: %s\n", strerror(errno));
        return 1;
      }
    } else if (pid > 0) {
      // Parent
      int status;
      waitpid(pid, &status, 0); // Waiting for child to end

      if (WIFEXITED(status)) {
        const int exit_code = WEXITSTATUS(status); // Save exit code
        printf("%s: pkill exit code: %d\n", UTIL_NAME, exit_code);
      }
    }
  }

  // Forking process
  pid_t pid = fork();

  // Error
  if (pid < 0) goto fork_fail;
  
  if (pid == 0) {
    // Child
    if (rehalt_unmount() != 0) {
      fprintf(stderr, "Failed to halt system: stage 2: unmount: %s\n", strerror(errno));
      return 1;
    }
  } else if (pid > 0) {
      // Parent
      int status;
      waitpid(pid, &status, 0); // Waiting for child to end
      
      if (WIFEXITED(status)) {
        const int exit_code = WEXITSTATUS(status); // Save exit code
        printf("%s: unmount exit code: %d\n", UTIL_NAME, exit_code);
      }
    }

  return 0;

fork_fail:
  fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
  return 1;
}
