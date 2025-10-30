#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h> // For gettimeofday and struct timeval
#include <unistd.h>
#include <signal.h>
#include <errno.h>

// Global flag to indicate if Ctrl-C was pressed
volatile sig_atomic_t stop_program = 0;
// Condition variable and mutex are no longer strictly needed for timing,
// but we keep the mutex to guard stop_program if we wanted to be perfectly safe,
// though volatile sig_atomic_t handles the flag safely on its own.
// We'll keep the condition variable only for the Ctrl-C signal handler to wake
// the main thread if it were waiting on something else, but here we simplify.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Total duration for the loop in seconds
#define MAX_SECONDS 20
// The desired interval (1 second)
#define WAIT_INTERVAL_SEC 1

// Structure to pass data to the reporter thread
typedef struct {
    struct timeval start_time;
} thread_data_t;


/**
 * Signal handler for SIGINT (Ctrl-C).
 * Sets the stop_program flag.
 * (We don't need to signal a condition variable if the reporter thread uses sleep/usleep).
 */
void ctrl_c_handler(int signum) {
    // We only need to set the stop flag here
    stop_program = 1;
}

/**
 * The dedicated thread function for status updates.
 * It uses a simple usleep-based loop to report elapsed time.
 */
void *status_reporter(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    struct timeval current_tv;
    long elapsed_time_sec = 0;
    long last_reported_sec = 0;

    // Wait until the total elapsed time reaches MAX_SECONDS or Ctrl-C is pressed
    while (!stop_program) {

        // Use gettimeofday to calculate elapsed time
        if (gettimeofday(&current_tv, NULL) == -1) {
            perror("gettimeofday in reporter thread");
            break;
        }

        // Calculate total elapsed seconds
        elapsed_time_sec = current_tv.tv_sec - data->start_time.tv_sec;

        // Check if a new second mark has been reached
        if (elapsed_time_sec > last_reported_sec) {

            // Check if we've hit the MAX_SECONDS limit
            if (elapsed_time_sec >= MAX_SECONDS) {
                // Let the main thread handle the final "Finished successfully" print
                break;
            }

            // Print the update
            printf("Second %ld elapsed...\n", elapsed_time_sec);
            fflush(stdout);

            // Update the last reported second
            last_reported_sec = elapsed_time_sec;
        }

        // Sleep for a short, consistent period (e.g., 10 milliseconds)
        // This is a simple, effective polling mechanism for this task.
        usleep(10000); // 10,000 microseconds = 10 milliseconds
    }

    return NULL;
}

int main() {
    pthread_t reporter_tid;
    thread_data_t thread_data;

    // 1. Set up the signal handler for Ctrl-C
    if (signal(SIGINT, ctrl_c_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    // Get the initial time
    if (gettimeofday(&thread_data.start_time, NULL) == -1) {
        perror("gettimeofday start");
        return 1;
    }

    printf("Starting loop for up to %d seconds (printing every %d second, or press Ctrl-C to stop).\n",
           MAX_SECONDS, WAIT_INTERVAL_SEC);

    // 2. Create the status reporter thread
    if (pthread_create(&reporter_tid, NULL, status_reporter, &thread_data) != 0) {
        perror("pthread_create");
        return 1;
    }

    // --- Main thread loop (simplified to just wait for termination condition) ---

    // The main thread now simply waits for either the MAX_SECONDS limit to be reached
    // or the stop_program flag to be set by Ctrl-C.

    struct timeval current_tv;
    long elapsed_time_sec = 0;

    // Use a basic loop and sleep/usleep/nanosleep or a condition variable wait
    // with a long timeout for the main thread to remain responsive but idle.
    while (elapsed_time_sec < MAX_SECONDS && !stop_program) {

        // Check current time
        if (gettimeofday(&current_tv, NULL) == -1) {
            perror("gettimeofday main loop");
            break;
        }
        elapsed_time_sec = current_tv.tv_sec - thread_data.start_time.tv_sec;

        // Sleep for a long time, allowing the reporter thread to do the work.
        // We use sleep() to yield the CPU for 1 second at a time (or less if the limit is close).
        long sleep_duration = (MAX_SECONDS - elapsed_time_sec > 0) ? (MAX_SECONDS - elapsed_time_sec) : 0;

        if (sleep_duration > 0 && !stop_program) {
            // Wait for up to 1 second or until MAX_SECONDS is hit.
            // If Ctrl-C is pressed, sleep might be interrupted, and the loop checks stop_program.
            sleep(sleep_duration > 1 ? 1 : sleep_duration);
        } else {
            // Exit loop if time is up or sleep_duration is 0
            break;
        }
    }

    // 3. Wait for the reporter thread to finish its work
    // By setting 'stop_program', we signal the reporter thread to exit its loop.
    if (stop_program) {
        printf("\nStop requested (Ctrl-C pressed).\n");
    }

    // Ensure the reporter thread exits if the main thread's loop finished first
    stop_program = 1;

    pthread_join(reporter_tid, NULL);

    // Cleanup (only mutex needs cleanup if used)
    pthread_mutex_destroy(&mutex);

    // Final message check
    if (elapsed_time_sec >= MAX_SECONDS && !stop_program) {
        printf("\nFinished successfully after %d seconds.\n", MAX_SECONDS);
    } else if (elapsed_time_sec >= MAX_SECONDS && stop_program) {
        // This case is rare but handles race conditions where Ctrl-C is pressed
        // at the exact moment the timer finishes.
        printf("\nFinished successfully after %d seconds.\n", MAX_SECONDS);
    }

    // Restore default signal handler
    signal(SIGINT, SIG_DFL);

    return 0;
}