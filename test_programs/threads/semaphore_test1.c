#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

static long shared = 0;
static sem_t sem;

enum {
    THREADS = 40
};

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

static void *threadFunc(void *arg) {
    long loops = *((long *) arg);

    for (long j = 0; j < loops; j++) {
        if (sem_wait(&sem) == -1)
            errExit("sem_wait");

        shared++;

        if (sem_post(&sem) == -1)
            errExit("sem_post");
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t[THREADS];
    int s;
    long nloops;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s num_loops\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    nloops = strtol(argv[1], NULL, 0);

    if (sem_init(&sem, 0, 1) == -1)
        errExit("sem_init");

    for (int i = 0; i < THREADS; ++i) {
        s = pthread_create(&t[i], NULL, threadFunc, &nloops);
        if (s != 0)
            errExit("pthread_create");
    }

    for (int i = 0; i < THREADS; ++i) {
        s = pthread_join(t[i], NULL);
        if (s != 0)
            errExit("pthread_join");
    }

    printf("shared = %ld\n", shared);
    exit(EXIT_SUCCESS);
}