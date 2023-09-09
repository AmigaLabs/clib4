#define _DEFAULT_SOURCE
#include <stdio.h>
#include <time.h>

int main() {
    time_t timestamp = 1675324800;

    if (stime(&timestamp) == -1) {
        perror("Error during stime call");
        return 1;
    }

    printf("Timestamp set\n");

    return 0;
}