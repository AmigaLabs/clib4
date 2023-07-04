/*
** file: measure.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h> /* gettimeofday() */
#include <pthread.h>
/*#include <sys/_structs.h>*/ /* defines time structures */

#define NUM_THREADS 10
#define DIGITS 4

typedef struct {
    int secs;
    int usecs;
} TIME_DIFF;

TIME_DIFF *my_difftime(struct timeval *, struct timeval *);

void *thread_function(void *);

int main(int argc, char *argv[]) {
    int iterations, i, rc;
    void *status;
    struct timeval myTVstart, myTVend;
    time_t clock_start, clock_end;
    TIME_DIFF *difference;
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;

    if (argc != 2) {
        fprintf(stderr, "Usage error. I am expecting one positive integer.\n");
        exit(1);
    }
    iterations = atoi(argv[1]);

    // Make threads Joinable for sure.
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    clock_start = clock();
    gettimeofday(&myTVstart, NULL);

    printf("Clock started counting!\n");

    for (i = 0; i < NUM_THREADS; i++) {
        rc = pthread_create(&(threads[i]), &attr, thread_function, (void *) &iterations);
        if (rc) {
            fprintf(stderr, "ERROR; return code from pthread_create() is &#37;d.\n", rc);
            exit(2);
        }
    }

    for (i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], &status);
    printf("All threads joined!\n");

    clock_end = clock();
    gettimeofday(&myTVend, NULL);
    printf("Clock stopped counting!\n\n");

    printf("clock start   : %10d.\n", (int) clock_start);
    printf("clock end     : %10d.\n", (int) clock_end);
    printf("CLOCKS_PER_SEC: %10d.\n", (int) CLOCKS_PER_SEC);
    printf("timeofday - start sec : %d.\n", (int) myTVstart.tv_sec);
    printf("timeofday - start usec: %6d.\n", (int) myTVstart.tv_usec);
    printf("timeofday - end sec   : %d.\n", (int) myTVend.tv_sec);
    printf("timeofday - end usec  : %6d.\n", (int) myTVend.tv_usec);
    printf("-----------------------------------------------------\n");
    printf("Time difference due to clock       : %10.6f secs.\n",
           (double) (clock_end - clock_start) / CLOCKS_PER_SEC);
    difference = my_difftime(&myTVstart, &myTVend);
    printf("Time difference due to gettimeofday: %3d.%6d secs.\n", difference->secs, difference->usecs);

    free(difference);

    return 0;
}

void *thread_function(void *input) {
    int loops = *((int *) input);
    int i;

    for (i = 0; i < loops; i++)
        rand() % RAND_MAX; // Just a dummy operation.

    pthread_exit(NULL);
}

TIME_DIFF *my_difftime(struct timeval *start, struct timeval *end) {
    TIME_DIFF *diff = (TIME_DIFF *) malloc(sizeof(TIME_DIFF));

    if (start->tv_sec == end->tv_sec) {
        diff->secs = 0;
        diff->usecs = end->tv_usec - start->tv_usec;
    } else {
        diff->usecs = 1000000 - start->tv_usec;
        diff->secs = end->tv_sec - (start->tv_sec + 1);
        diff->usecs += end->tv_usec;
        if (diff->usecs >= 1000000) {
            diff->usecs -= 1000000;
            diff->secs += 1;
        }
    }

    return diff;
}