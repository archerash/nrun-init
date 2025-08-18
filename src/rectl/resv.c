#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

int resv(int attempts, const int sleep_time, const char *path) {
  while (attempts-- > 0) {
    pid_t pid = fork(); // create a new child process

    if (pid == 0) {
      // Child process
      execl(path, path, (char *)NULL);
      // If execvp returns, an error occurred
      perror("execvp failed");
      _exit(127);

    } else if (pid > 0) {
      // Parent process
      int status;
      int alive = 1;

      while (alive) {
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == 0) {
          // Child is still running
          sleep(1);
        } else if (result == pid) {
          // Child has finished
          alive = 0;
          if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Process ended normally with code 0\n");
            return 0;
          } else {
            int code = WIFEXITED(status) ? WEXITSTATUS(status) : 128 + WTERMSIG(status);
            printf("Process ended with code: %d\n", code);
          }
        } else {
          // waitpid error
          perror("waitpid failed");
          break;
        }
      }

      // If attempts remain, sleep before retrying
      if (attempts > 0) {
        printf("Retrying in %d seconds...\n", sleep_time);
        sleep(sleep_time);
      } else {
        printf("No more attempts left\n");
        return 1;
      }

    } else {
      // Fork failed
      perror("fork failed");
      return 1;
    }
  }

  return 1;
}
