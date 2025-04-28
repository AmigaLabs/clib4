#include <stdio.h>
#include <unistd.h>

int main() {
    printf("unix paths enabled: %d\n", unixPathsEnabled());

    return 0;
}