#include <pthread.h>
#include <stdio.h>

#define SIZE 1000000

#ifdef THREAD
#define THR __thread
#else
#define THR
#endif

THR int i;

void *fn(void *) {
    i = 0;
    while (i++ < SIZE);
        printf("Created thread val of i : %i\n", i);
}

int main(void) {
    pthread_t t;
    if (pthread_create(&t, NULL, fn, NULL)) {
        return 10;
    }

    if (pthread_join(t, NULL)) {
        return 10;
    }

    printf("Parent thread val of i  : %i\n", i);
    return 0;
}