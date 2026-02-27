#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char *argv[]) {

    if (argc < 2) {
        printf("Usage: spawnv <command>\n");
        return 1;
    }

    int rc = spawnv(_P_WAIT, argv[1], &argv[1]);
    if (rc == 0) {
        printf("spawnv: %s executed correctly\n", argv[1]);
    }
    else {
        printf("Error running %s\n", argv[1]);
    }

    return 0;
}