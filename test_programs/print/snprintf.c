#include <stdio.h>

#define MAJOR 1
#define MINOR 1
#define REVISION 5

int main() {
    char version[50] = {0};
    int v = 21;

    snprintf(version, 49, "%d.%d wrapper %d.%d.%d", v / 10, v % 10, MAJOR, MINOR, REVISION);
    printf("version = %s\n", version);

    return 0;
}