#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>

#define ASIZE 1024

int main(int argc, char *argv[]) {
    int i;
    long rand_number[ASIZE] = {0};
    unsigned short seed[3] = {155, 0, 155};

    // Initialize Value For Random Number
    seed48(&seed[0]);

    for (i = 0; i < ASIZE; i++) {
        rand_number[i] = lrand48();
    }

    // Sample Array Element
    printf("%ld\n", rand_number[ASIZE - 1]);
    return 0;
}