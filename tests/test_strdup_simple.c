#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char *result = strdup("hello");
    if (result) {
        printf("strdup worked: %s\n", result);
        free(result);
    } else {
        printf("strdup returned NULL\n");
    }
    return 0;
}
