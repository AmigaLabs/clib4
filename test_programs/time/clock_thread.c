#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <proto/dos.h>

// Thread function
void* thread_function(void* arg) {
    struct timespec ts;

    // Get the current time using clock_gettime()
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
        perror("clock_gettime");
        pthread_exit(NULL);
    }

    // Print the time in seconds and nanoseconds
    printf("Thread: Current time is %ld seconds and %ld nanoseconds\n", ts.tv_sec, ts.tv_nsec);

    // Simulate some work with sleep
    sleep(1);

    pthread_exit(NULL);
}

int main() {
    pthread_t thread_id;

    // Create a new thread
    if (pthread_create(&thread_id, NULL, thread_function, NULL) != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    // Wait for the thread to finish
    if (pthread_join(thread_id, NULL) != 0) {
        perror("pthread_join");
        return EXIT_FAILURE;
    }

    printf("Main: Thread has finished execution.\n");

    return EXIT_SUCCESS;
}
