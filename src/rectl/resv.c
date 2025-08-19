#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int resv(int attempts, int sleep_time, const char *cmd) {
  if (attempts < 0 || sleep_time < 0 || cmd == NULL) {
    fprintf(stderr, "Invalid input\n");
    return 1;
  }

  for (int i = 0; i < attempts; i++) {
    printf("%d: run: \"%s\"\n", i + 1, cmd);

    // Copying string to tokenize it
    char *copy = strdup(cmd);
    if (copy == NULL) return -1;

    // Dynamic argv array
    size_t argv_size = 4; // default size
    char **argv = malloc(argv_size * sizeof(char*));
    if (argv == NULL) {
      free(copy);
      return -1;
    }

    size_t argc = 0;
    char *token = strtok(copy, " \t");
    while (token) {
      if (argc + 1 >= argv_size) {
        argv_size *= 2;
        char **tmp = realloc(argv, argv_size * sizeof(char*));
        if (tmp = NULL) {
          free(argv);
          free(copy);
          return -1;
        }
        argv = tmp;
      }
      argv[argc++] = token;
      token = strtok(NULL, " \t");
    }
    argv[argc] = NULL;

    if (argc == 0) {
      free(argv);
      free(copy);
      return -1; 
    }

    // Forking process
    pid_t pid = fork();
    if (pid < 0) {
      fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
      free(argv);
      free(copy);
      return -1;
    }

    if (pid == 0) {
      // Running command
      execvp(argv[0], argv);
      perror("execvp failed");
      _exit(127);
    }

    int status = -1;
    if (waitpid(pid, &status, 0) < 0) {
      perror("waitpid failed");
      free(argv);
      free(copy);
      return -1;
    }

    free(argv);
    free(copy);

    // If it succeded
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
      printf("Process exited with exit code: 0\n");
      return 0;  
    }

    fprintf(stderr, "Process failed: Exit code: %d\n", WIFEXITED(status) ? WEXITSTATUS(status) : -1);

    if (i < attempts - 1) sleep(sleep_time);
    
  }

  fprintf(stderr, "All %d attempts failed.\n", attempts);
  return -1;
}
