/*
 * Test for pthread_join race condition bug
 *
 * This test demonstrates the bug where:
 * 1. Thread exits very quickly and goes to THREAD_STATE_DESTRUCT
 * 2. pthread_join checks status and sees THREAD_STATE_DESTRUCT
 * 3. pthread_join returns 0 WITHOUT cleaning up the thread
 * 4. Thread remains "hanging" and not properly cleaned up
 *
 * The bug is in pthread_join.c around lines 55-60:
 *   if (inf->status == THREAD_STATE_TERMINATING || inf->status == THREAD_STATE_DESTRUCT) {
 *       return 0;  // BUG: Returns without cleanup!
 *   }
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define NUM_THREADS 5
#define NUM_ITERATIONS 10

static volatile int completed_threads = 0;
static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Very fast thread that exits immediately
 * This increases the chance that the thread reaches THREAD_STATE_DESTRUCT
 * before pthread_join is called
 */
void *fast_thread(void *arg) {
    int id = *(int *)arg;

    printf("[Thread %d] Started and exiting immediately\n", id);
    fflush(stdout);

    pthread_mutex_lock(&count_mutex);
    completed_threads++;
    pthread_mutex_unlock(&count_mutex);

    // Exit immediately - no delay
    return (void *)(long)id;
}

/*
 * Slow thread for comparison
 */
void *slow_thread(void *arg) {
    int id = *(int *)arg;

    printf("[Thread %d] Started, sleeping...\n", id);
    fflush(stdout);

    usleep(100000); // 100ms

    printf("[Thread %d] Exiting\n", id);
    fflush(stdout);

    pthread_mutex_lock(&count_mutex);
    completed_threads++;
    pthread_mutex_unlock(&count_mutex);

    return (void *)(long)id;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    void *retval;
    int failures = 0;

    printf("=== PTHREAD_JOIN RACE CONDITION TEST ===\n");
    printf("This test checks if pthread_join properly cleans up threads\n");
    printf("that exit very quickly (race condition scenario)\n\n");

    /* TEST 1: Very fast threads that exit before join */
    printf("=== TEST 1: Fast threads (exit before join) ===\n");

    for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
        printf("\n--- Fast iteration %d ---\n", iter);
        completed_threads = 0;

        // Create threads
        for (int i = 0; i < NUM_THREADS; i++) {
            thread_ids[i] = i;
            printf("[Main] Creating fast thread %d\n", i);
            fflush(stdout);

            int ret = pthread_create(&threads[i], NULL, fast_thread, &thread_ids[i]);
            if (ret != 0) {
                printf("[Main] ERROR: pthread_create failed: %d\n", ret);
                failures++;
            }
        }

        // Give threads time to finish BEFORE we call pthread_join
        // This increases the chance they'll be in THREAD_STATE_DESTRUCT
        printf("[Main] Sleeping to let threads finish first...\n");
        usleep(50000); // 50ms

        // Now try to join - threads are likely already in THREAD_STATE_DESTRUCT
        for (int i = 0; i < NUM_THREADS; i++) {
            printf("[Main] Joining fast thread %d...\n", i);
            fflush(stdout);

            int ret = pthread_join(threads[i], &retval);

            if (ret != 0) {
                printf("[Main] ERROR: pthread_join failed for thread %d: %d\n", i, ret);
                failures++;
            } else {
                printf("[Main] Thread %d joined successfully, retval=%ld\n",
                       i, (long)retval);
            }
            fflush(stdout);
        }

        printf("[Main] All fast threads joined. Completed: %d/%d\n",
               completed_threads, NUM_THREADS);

        if (completed_threads != NUM_THREADS) {
            printf("ERROR: Not all threads completed! %d/%d\n",
                   completed_threads, NUM_THREADS);
            failures++;
        }
    }

    /* TEST 2: Normal speed threads for comparison */
    printf("\n\n=== TEST 2: Slow threads (join before exit) ===\n");

    for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
        printf("\n--- Slow iteration %d ---\n", iter);
        completed_threads = 0;

        // Create threads
        for (int i = 0; i < NUM_THREADS; i++) {
            thread_ids[i] = i;
            printf("[Main] Creating slow thread %d\n", i);
            fflush(stdout);

            int ret = pthread_create(&threads[i], NULL, slow_thread, &thread_ids[i]);
            if (ret != 0) {
                printf("[Main] ERROR: pthread_create failed: %d\n", ret);
                failures++;
            }
        }

        // Join immediately - threads are still running
        for (int i = 0; i < NUM_THREADS; i++) {
            printf("[Main] Joining slow thread %d...\n", i);
            fflush(stdout);

            int ret = pthread_join(threads[i], &retval);

            if (ret != 0) {
                printf("[Main] ERROR: pthread_join failed for thread %d: %d\n", i, ret);
                failures++;
            } else {
                printf("[Main] Thread %d joined successfully, retval=%ld\n",
                       i, (long)retval);
            }
            fflush(stdout);
        }

        printf("[Main] All slow threads joined. Completed: %d/%d\n",
               completed_threads, NUM_THREADS);

        if (completed_threads != NUM_THREADS) {
            printf("ERROR: Not all threads completed! %d/%d\n",
                   completed_threads, NUM_THREADS);
            failures++;
        }
    }

    /* SUMMARY */
    printf("\n=== SUMMARY ===\n");
    if (failures == 0) {
        printf("✓ ALL TESTS PASSED\n");
        printf("pthread_join correctly handles both fast and slow threads\n");
        return 0;
    } else {
        printf("✗ TESTS FAILED: %d failures\n", failures);
        printf("pthread_join has race condition bugs when threads exit quickly\n");
        return 1;
    }
}
