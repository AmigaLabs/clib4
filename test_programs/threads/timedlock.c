#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

pthread_mutex_t lock; /* exclusive lock */

//do some work to keep the processor busy..
int busy() {
    int x = 0;
    for (int i = 0; i < 1024 * 1024 * 10; i++)
        x += 1;
    return x;
}

//lock mutex, wait at maximum 60 seconds, return sucesss
int timedLock() {
    struct timespec timeoutTime;
    clock_gettime(CLOCK_REALTIME, &timeoutTime);
    timeoutTime.tv_sec += 60;

    printf("Nanoseconds: %ld, Seconds %ld\n", timeoutTime.tv_nsec, timeoutTime.tv_sec);
    int retVal = pthread_mutex_timedlock(&lock, &timeoutTime);
    printf("pthread_mutex_timedlock(): %d\n", retVal);
    if (retVal != 0) {
        const char *errVal = NULL;
        switch (retVal) {
            case EINVAL:
                errVal = "EINVAL";
                break;
            case EAGAIN:
                errVal = "EAGAIN";
                break;
            case ETIMEDOUT:
                errVal = "ETIMEDOUT";
                break;
            case EDEADLK:
                errVal = "EDEADLK";
                break;
            default:
                errVal = "unknown..";
                break;
        }
        printf("Error taking lock in thread %ld: %s (%s)\n", pthread_self(), errVal, strerror(retVal));
    }
    return retVal == 0; //indicate success/failure
}

void testLockNative() {
    uint32_t thread_id = pthread_self();
    printf("Trying to take lock in thread %lu.\n", thread_id);
    int ret = timedLock();
    printf("Got lock in thread %lu. sucess=%d\n", thread_id, ret);
    busy();
    printf("Giving up lock now from thread %lu.\n", thread_id);
    pthread_mutex_unlock(&lock);
}

void *test_thread(void *arg) {
    testLockNative();
    return NULL;
}

int main() {
    pthread_mutex_init(&lock, NULL);
    //create two threads which will try to access the protected function at once
    pthread_t t1, t2;
    pthread_create(&t1, NULL, &test_thread, NULL);
    pthread_create(&t2, NULL, &test_thread, NULL);

    //wait for threads to end
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}