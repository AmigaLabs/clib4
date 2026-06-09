/*
 * Test pthread_getspecific/setspecific called after pthread library destruction
 *
 * This test verifies that calling pthread_getspecific or pthread_setspecific
 * from a global destructor (after __pthread_exit_func has been called) does
 * not crash but returns safely.
 *
 * Scenario:
 * 1. Create pthread key and set thread-specific data
 * 2. Register a global destructor that runs AFTER pthread cleanup
 * 3. In that destructor, try to call pthread_getspecific/setspecific
 * 4. Verify it returns error instead of crashing
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

static pthread_key_t test_key;
static int destructor_called = 0;
static int late_call_result = -99;  /* Will be set by late destructor */

/* This destructor runs for thread-specific data */
void thread_data_destructor(void *data) {
    printf("[TLS Destructor] Called with data=%p\n", data);
    destructor_called++;

    /* Try to access pthread_getspecific from within destructor */
    /* This should work because pthread is still initialized */
    void *retrieved = pthread_getspecific(test_key);
    printf("[TLS Destructor] pthread_getspecific returned: %p (expected: %p)\n",
           retrieved, data);

    if (data) {
        free(data);
    }
}

/* Global object whose destructor runs LATE (after pthread cleanup) */
typedef struct {
    int dummy;
} LateDestructor;

static LateDestructor late_obj = {42};

/* This runs AFTER pthread library cleanup (__pthread_exit_func) */
__attribute__((destructor(101)))  /* Lower priority = runs later */
void late_destructor(void) {
    printf("\n[Late Destructor] Called AFTER pthread cleanup\n");
    printf("[Late Destructor] Attempting pthread_getspecific...\n");

    /* This should NOT crash, even though pthread is cleaned up */
    /* It should return NULL safely */
    void *result = pthread_getspecific(test_key);
    printf("[Late Destructor] pthread_getspecific returned: %p (should be NULL)\n", result);

    if (result == NULL) {
        printf("[Late Destructor] + PASS: Returned NULL safely\n");
        late_call_result = 0;  /* Success */
    } else {
        printf("[Late Destructor] x FAIL: Returned non-NULL: %p\n", result);
        late_call_result = 1;  /* Failure */
    }

    /* Also try pthread_setspecific */
    printf("[Late Destructor] Attempting pthread_setspecific...\n");
    int set_result = pthread_setspecific(test_key, (void*)0x12345);
    printf("[Late Destructor] pthread_setspecific returned: %d (should be EINVAL=%d)\n",
           set_result, EINVAL);

    if (set_result == EINVAL) {
        printf("[Late Destructor] + PASS: Returned EINVAL safely\n");
    } else {
        printf("[Late Destructor] x FAIL: Returned %d instead of EINVAL\n", set_result);
        late_call_result = 1;
    }

    printf("[Late Destructor] Completed without crash!\n");
}

void *thread_func(void *arg) {
    int thread_id = *(int *)arg;

    printf("[Thread %d] Starting\n", thread_id);

    /* Allocate and set thread-specific data */
    void *data = malloc(64);
    if (data) {
        snprintf((char *)data, 64, "Thread %d data", thread_id);

        int result = pthread_setspecific(test_key, data);
        if (result == 0) {
            printf("[Thread %d] Set thread-specific data: %p\n", thread_id, data);
        } else {
            printf("[Thread %d] Failed to set data: %d\n", thread_id, result);
            free(data);
        }
    }

    /* Verify we can retrieve it */
    void *retrieved = pthread_getspecific(test_key);
    printf("[Thread %d] Retrieved data: %p (expected: %p)\n", thread_id, retrieved, data);

    usleep(10000);  /* Let other threads run */

    printf("[Thread %d] Exiting\n", thread_id);
    return NULL;
}

int main(void) {
    pthread_t threads[3];
    int thread_ids[3];
    int result;

    printf("=== PTHREAD DESTRUCTOR ORDER TEST ===\n");
    printf("Testing pthread_getspecific/setspecific after library cleanup\n\n");

    /* Create key with destructor */
    result = pthread_key_create(&test_key, thread_data_destructor);
    if (result != 0) {
        printf("Failed to create key: %d\n", result);
        return 1;
    }
    printf("[Main] Created pthread key with destructor\n\n");

    /* Create threads that use the key */
    for (int i = 0; i < 3; i++) {
        thread_ids[i] = i;
        result = pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        if (result != 0) {
            printf("[Main] Failed to create thread %d: %d\n", i, result);
            return 1;
        }
    }

    /* Wait for all threads */
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n[Main] All threads joined\n");
    printf("[Main] Thread-specific destructors called: %d times\n", destructor_called);

    /* Also set data in main thread */
    void *main_data = malloc(64);
    if (main_data) {
        snprintf((char *)main_data, 64, "Main thread data");
        pthread_setspecific(test_key, main_data);
        printf("[Main] Set main thread data: %p\n", main_data);
    }

    printf("\n[Main] Exiting - pthread cleanup will run, then late_destructor\n");
    printf("[Main] The late_destructor will test accessing pthread after cleanup\n\n");

    /* When main exits, destructors run in this order:
     * 1. Thread-specific data destructors (via __pthread_exit_func)
     * 2. __pthread_exit_func cleans up pthread library
     * 3. late_destructor runs and tries to use pthread (should not crash)
     */

    return 0;
}
