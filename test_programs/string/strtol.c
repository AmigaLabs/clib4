#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h> /* gettimeofday() */

typedef struct {
    int secs;
    int usecs;
} TIME_DIFF;

TIME_DIFF *my_difftime(struct timeval *, struct timeval *);

int main(int argc, const char **argv) {
    struct timeval myTVstart, myTVend;
    time_t clock_start, clock_end;
    TIME_DIFF *difference;

    if (argc != 3) {
        printf("Usage: %s <integer> <iterations>\n", argv[0]);
        return -1;
    }

    const char *input = argv[1];
    const char *iteration_s = argv[2];

    long long result = 0;
    size_t i;

    int iterations = strtol(argv[2], (char **) NULL, 10);
    if (ERANGE == errno) {
        printf("Overflow when parsing iterations\n");
        return -1;
    }

    if (iterations < 0) {
        printf("Iterations must be positive\n");
        return -1;
    }

    clock_start = clock();
    gettimeofday(&myTVstart, NULL);

    for (i = iterations; i; i--) {
        result = strtoll(input, (char **) NULL, 10);
    }

    clock_end = clock();
    gettimeofday(&myTVend, NULL);

    printf("Finished conversion\n");
    printf("Time difference due to clock       : %10.6f secs.\n", (double) (clock_end - clock_start) / CLOCKS_PER_SEC);
    difference = my_difftime(&myTVstart, &myTVend);
    printf("Time difference due to gettimeofday: %3d.%6d secs.\n", difference->secs, difference->usecs);

    free(difference);

    return 0;
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