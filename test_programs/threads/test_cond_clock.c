#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

int main() {
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    struct timespec abstime, current;
    int ret;

    printf("Test: pthread_cond_timedwait with CLOCK_REALTIME (default)\n\n");

    // Get current time using CLOCK_REALTIME
    clock_gettime(CLOCK_REALTIME, &current);
    printf("Current time (CLOCK_REALTIME): %ld.%09ld\n", current.tv_sec, current.tv_nsec);

    // Set timeout to 1 second in the future
    abstime.tv_sec = current.tv_sec + 1;
    abstime.tv_nsec = current.tv_nsec;
    printf("Timeout time: %ld.%09ld (1 second from now)\n", abstime.tv_sec, abstime.tv_nsec);

    pthread_mutex_lock(&mutex);

    printf("\nCalling pthread_cond_timedwait...\n");
    time_t start = time(NULL);
    ret = pthread_cond_timedwait(&cond, &mutex, &abstime);
    time_t end = time(NULL);

    printf("Returned after %ld seconds with code: %d\n", end - start, ret);

    if (ret == ETIMEDOUT) {
        printf("+ PASS: Timed out as expected after ~1 second\n");
        if ((end - start) >= 1 && (end - start) <= 2) {
            printf("+ PASS: Timing is correct (1 second +/- 1)\n");
        } else {
            printf("X FAIL: Timing is wrong (expected 1 second, got %ld)\n", end - start);
            return 1;
        }
    } else {
        printf("X FAIL: Expected ETIMEDOUT (%d), got %d\n", ETIMEDOUT, ret);
        return 1;
    }

    pthread_mutex_unlock(&mutex);

    printf("\nTest passed!\n");
    return 0;
}
