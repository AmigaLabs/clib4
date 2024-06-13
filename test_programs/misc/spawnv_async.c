#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[]) {

    if (argc < 2) {
        printf("Usage: spawnv_async <command>\n");
        return 1;
    }
    int pid = spawnv(P_NOWAIT, argv[1], &argv[1]);
    if (pid )
    printf("spawnv PID %d\n", pid);

    return 0;
}