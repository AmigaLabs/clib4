#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

extern char **environ;

void run_cmd(char *cmd) {
    pid_t pid;
    char *argv[] = {"run", cmd, NULL};
    int status;
    printf("Run command: %s\n", cmd);
    status = posix_spawn(&pid, "newshell ", NULL, NULL, argv, environ);
    if (status == 0) {
        printf("Child pid: %i\n", pid);
        do {
            int ret = waitpid(pid, &status, 0);
            printf("waitpid ret = %d\n", ret);
            if (ret != -1) {
                printf("Child status %d\n", WEXITSTATUS(status));
            } else {
                perror("waitpid");
                //exit(1);
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    } else {
        printf("posix_spawn: %s\n", strerror(status));
    }
}

void main(int argc, char *argv[]) {
    run_cmd(argv[1]);
    return;
}