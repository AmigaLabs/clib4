#include <stdlib.h>
#include <stdio.h>

int num;

int ind, newsize, i = 1;
#define BLOCK_SIZE 1024 * 1024 * 10

void *expandArray(void *arg) {
    int *a = (int *) arg;
    int *b = NULL;
    while (newsize < num) {
        newsize = newsize + BLOCK_SIZE;
        b = realloc(a, sizeof(int) * newsize);
        b[newsize - 1] = i;
        a = b;
    }
    if (b != NULL)
        free(b);
    return 0;
}

int main(int argc, char **argv) {
    num = 1000000;
    if (!(num > 0 && num <= 1000000)) {
        printf("BAD NUMBER\n");
        return 0;
    }
    int *a = (int *) malloc(sizeof(int));
    if (a == NULL) {
        return 0;
    }
    newsize = 0;
    expandArray(a);
    free(a);
    return 0;
}