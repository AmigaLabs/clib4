#include <stdlib.h>
#include <stdio.h>

int num, ind, newsize, i = 1;
#define BLOCK_SIZE 1024 * 1024 * 10

void *expandArray(int **pointer) {
    while (newsize < num) {
        newsize = newsize + BLOCK_SIZE;
        int *temp = realloc(*pointer, sizeof(int) * newsize);
        if (temp != NULL) {
            temp[newsize - 1] = i;
            *pointer = temp;
        } else {
            return 0;
        }
    }

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
    expandArray(&a);
    free(a);
    return 0;
}