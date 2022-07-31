#define _GNU_SOURCE
#include <crypt.h>
#include <stdio.h>

int main(void) {
    struct crypt_data data[1] = {0};
    char *res;

    res = crypt_r("password", "QX", data);
    printf("return value from crypt_r was %s\n", res);
    return 0;
}