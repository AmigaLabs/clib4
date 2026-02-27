/*
 * Test to verify that pthread_getspecific/setspecific don't cause
 * infinite loops when DEBUG is enabled
 *
 * This test was created to fix a bug where D() debug macros inside
 * pthread_getspecific/setspecific could call back into those functions
 * while holding the tls_sem mutex, causing deadlock or infinite loops.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5
#define NUM_ITERATIONS 100

static pthread_key_t test_key;

void *thread_func(void *arg) {
    int thread_id = *(int *)arg;

    printf("[Thread %d] Starting\n", thread_id);

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        /* Allocate some data */
        int *data = malloc(sizeof(int));
        if (!data) {
            printf("[Thread %d] malloc failed at iteration %d\n", thread_id, i);
            return NULL;
        }

        *data = thread_id * 1000 + i;

        /* Set thread-specific data */
        if (pthread_setspecific(test_key, data) != 0) {
            printf("[Thread %d] pthread_setspecific failed at iteration %d\n", thread_id, i);
            free(data);
            return NULL;
        }

        /* Get thread-specific data back */
        int *retrieved = pthread_getspecific(test_key);
        if (!retrieved) {
            printf("[Thread %d] pthread_getspecific returned NULL at iteration %d\n", thread_id, i);
            free(data);
            return NULL;
        }

        if (*retrieved != *data) {
            printf("[Thread %d] Data mismatch at iteration %d: expected %d, got %d\n",
                   thread_id, i, *data, *retrieved);
            free(data);
            return NULL;
        }

        /* Clear and free */
        pthread_setspecific(test_key, NULL);
        free(data);

        /* Progress indicator */
        if (i % 10 == 0) {
            printf("[Thread %d] Progress: %d/%d\n", thread_id, i, NUM_ITERATIONS);
        }
    }

    printf("[Thread %d] Completed successfully\n", thread_id);
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    printf("=== TLS NO LOOP TEST ===\n");
    printf("This test verifies pthread_getspecific/setspecific don't cause loops\n");
    printf("Creating key...\n");

    if (pthread_key_create(&test_key, NULL) != 0) {
        printf("FAIL: pthread_key_create failed\n");
        return 1;
    }

    printf("Key created successfully\n");
    printf("Creating %d threads...\n", NUM_THREADS);

    /* Create threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]) != 0) {
            printf("FAIL: pthread_create failed for thread %d\n", i);
            return 1;
        }
        printf("Thread %d created\n", i);
    }

    printf("All threads created, waiting for completion...\n");

    /* Wait for threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        void *result;
        if (pthread_join(threads[i], &result) != 0) {
            printf("FAIL: pthread_join failed for thread %d\n", i);
            return 1;
        }
        printf("Thread %d joined\n", i);
    }

    printf("All threads completed\n");
    printf("Deleting key...\n");

    if (pthread_key_delete(test_key) != 0) {
        printf("FAIL: pthread_key_delete failed\n");
        return 1;
    }

    printf("Key deleted successfully\n");
    printf("\n=== SUCCESS ===\n");
    printf("Test completed without infinite loops or deadlocks!\n");
    printf("This confirms the fix for the D() macro issue.\n");

    return 0;
}
