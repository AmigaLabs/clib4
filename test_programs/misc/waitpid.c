#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    int status;

    if (argc < 2) {
        printf("Usage: waitpid <command>\n");
        return 1;
    }
    printf("Run command: %s\n", argv[1]);
    pid = spawnv(P_NOWAIT, argv[1], (const char **) &argv[1]);
    if (pid > 0) {
        printf("Child pid: %i\n", pid);
        do {
            int ret = waitpid(pid, &status, WNOHANG);
            printf("waitpid ret = %d\n", ret);
            if (ret != -1) {
                printf("Child status %d\n", WEXITSTATUS(status));
            } else {
                perror("waitpid");
                break;
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    } else {
        printf("spawnv: %s\n", strerror(status));
    }
}