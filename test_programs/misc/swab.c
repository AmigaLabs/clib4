#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/byteswap.h>

int main() {
    const char *source = "oCedoCsgi  srgae t";
    char target[19] = {0};
    swab(source, target, strlen(source));
    printf("swab(%s) becomes (%s)", source, target);
    return 0;
}