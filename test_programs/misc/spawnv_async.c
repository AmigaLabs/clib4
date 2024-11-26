#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char *argv[]) {

    if (argc < 2) {
        printf("Usage: spawnv_async <command>\n");
        return 1;
    }
    int pid = spawnv(P_NOWAIT, argv[1], &argv[1]);
    if (pid) {
        printf("spawnv_async PID %d\n", pid);
        wait(&pid);
    }
    else {
        printf("Error running %s\n", argv[1]);
    }

    return 0;
}