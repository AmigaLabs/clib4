#include <string.h>
#include <stdio.h>

int main() {
    const char *foo = "NULL TEST COMPARISON";

    int ret = strcmp(foo, NULL);
    printf("ret = %d\n", ret);
}