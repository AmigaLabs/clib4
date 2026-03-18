#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

// Global flag to indicate if Ctrl-C was pressed
volatile sig_atomic_t stop_program = 0;
// Condition variable and mutex
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Total duration for the loop in seconds
#define MAX_SECONDS 20
// The desired interval (1 second)
#define WAIT_INTERVAL_SEC 1

/**
 * Signal handler for SIGINT (Ctrl-C).
 * Sets the stop_program flag and signals the condition variable to wake the main thread.
 */
void ctrl_c_handler(int signum) {
    stop_program = 1;
    // Wake up the waiting thread immediately
    pthread_cond_signal(&cond); 
}

int main() {
    struct timespec start_time, current_time, timeout_abs;
    long elapsed_time_sec = 0;
    long remaining_sec;
    int ret;

    // 1. Set up the signal handler for Ctrl-C using signal()
    if (signal(SIGINT, ctrl_c_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    // Get the initial time
    if (clock_gettime(CLOCK_REALTIME, &start_time) == -1) {
        perror("clock_gettime start");
        return 1;
    }

    printf("Starting loop for up to %d seconds (printing every %d second, or press Ctrl-C to stop).\n", 
           MAX_SECONDS, WAIT_INTERVAL_SEC);

    // Lock the mutex for the wait loop
    pthread_mutex_lock(&mutex);

    // Loop until the total elapsed time reaches MAX_SECONDS or Ctrl-C is pressed
    while (elapsed_time_sec < MAX_SECONDS && !stop_program) {
        
        // 2. Calculate remaining time and next absolute timeout
        
        // Calculate total elapsed seconds so far
        if (clock_gettime(CLOCK_REALTIME, &current_time) == -1) {
            perror("clock_gettime current");
            break;
        }
        elapsed_time_sec = current_time.tv_sec - start_time.tv_sec;

        // Determine how many more full seconds we need to wait
        // The print should happen when we reach i+1 seconds, where i is 0, 1, 2, ...
        long next_print_time_sec = elapsed_time_sec + WAIT_INTERVAL_SEC;

        if (next_print_time_sec > MAX_SECONDS) {
            // We're in the final stretch, wait only until MAX_SECONDS
            remaining_sec = MAX_SECONDS - elapsed_time_sec;
        } else {
            // Wait for the full interval, based on the next second mark
            remaining_sec = WAIT_INTERVAL_SEC;
        }

        // If no time is left, break the loop
        if (remaining_sec <= 0) {
            break;
        }
        
        // Calculate the absolute time for the next wakeup
        timeout_abs.tv_sec = current_time.tv_sec + remaining_sec;
        timeout_abs.tv_nsec = current_time.tv_nsec;

        // 3. Perform the timed wait
        // This is the point in time we expect to reach a full second mark (or Ctrl-C)
    	ret = pthread_cond_timedwait(&cond, &mutex, &timeout_abs);

        // 4. Check results and update
        if (ret == 0 && stop_program) {
            // Wakeup was due to signal (Ctrl-C)
            printf("\nStop requested (Ctrl-C pressed).\n");
            break;
        } 
        
        // Re-calculate the actual elapsed time after the wait (whether timeout, spurious, or signal)
        if (clock_gettime(CLOCK_REALTIME, &current_time) == -1) {
            perror("clock_gettime after wait");
            break;
        }
        elapsed_time_sec = current_time.tv_sec - start_time.tv_sec;

        // If the elapsed time has reached a full second mark, print the line
        // This is the **crucial check** to handle spurious wakeups and signal wakeups
        if (elapsed_time_sec >= next_print_time_sec && elapsed_time_sec <= MAX_SECONDS) {
            printf("Second %ld elapsed...\n", elapsed_time_sec);
            fflush(stdout); 
        } 
        // If elapsed_time_sec is still less than next_print_time_sec, it was a spurious
        // wakeup or a very short signal delay. The loop continues and calculates a new
        // and shorter 'remaining_sec' for the next wait, effectively ignoring the event.
    }
    
    // Unlock and cleanup
    pthread_mutex_unlock(&mutex);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    if (elapsed_time_sec >= MAX_SECONDS && !stop_program) {
        printf("\nFinished successfully after %d seconds.\n", MAX_SECONDS);
    }

    signal(SIGINT, SIG_DFL);

    return 0;
}
