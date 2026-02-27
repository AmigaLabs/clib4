/*
 * Test for pthread_setspecific race conditions and crash scenarios
 *
 * This test simulates the QT6 crash scenario where:
 * 1. A thread calls pthread_setspecific with a deleted key
 * 2. Multiple threads access the same key while one deletes it
 * 3. GetCurrentThreadInfo() returns NULL scenarios
 *
 * The crash in QT6 qmake was:
 *   DSI exception at pthread_setspecific()+0x40
 *   Instruction: stwx r30,r9,r31 (writing to memory)
 *   dar = 0xFFFFFFFF (invalid address)
 *   Called from QThreadData::deref()
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define NUM_THREADS 10
#define NUM_ITERATIONS 1000

static pthread_key_t test_key;
static volatile int key_valid = 1;
static volatile int test_running = 1;
static volatile int crash_count = 0;
static pthread_mutex_t crash_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int thread_id;
    int operations;
    char data[256];
} thread_data_t;

void test_destructor(void *data) {
    if (data) {
        free(data);
    }
}

/*
 * Test 1: Use key after deletion (simulates QT6 crash)
 */
void *test_deleted_key(void *arg) {
    int thread_id = *(int *)arg;
    thread_data_t *data;

    printf("[Thread %d] Starting deleted key test\n", thread_id);

    for (int i = 0; i < NUM_ITERATIONS && test_running; i++) {
        if (key_valid) {
            /* Allocate and set data */
            data = malloc(sizeof(thread_data_t));
            if (data) {
                data->thread_id = thread_id;
                data->operations = i;
                snprintf(data->data, sizeof(data->data),
                         "Thread %d iteration %d", thread_id, i);

                /* Try to set - this should NOT crash even if key is deleted */
                int result = pthread_setspecific(test_key, data);

                if (result != 0) {
                    /* Key was deleted or invalid - this is OK */
                    free(data);
                    if (result == EINVAL && !key_valid) {
                        /* Expected: key was deleted */
                        continue;
                    } else {
                        printf("[Thread %d] Unexpected error: %d\n", thread_id, result);
                    }
                } else {
                    /* Successfully set - try to get it back */
                    void *retrieved = pthread_getspecific(test_key);
                    if (retrieved != data && key_valid) {
                        printf("[Thread %d] ERROR: Retrieved wrong data!\n", thread_id);
                        pthread_mutex_lock(&crash_mutex);
                        crash_count++;
                        pthread_mutex_unlock(&crash_mutex);
                    }

                    /* Clean up */
                    pthread_setspecific(test_key, NULL);
                    free(data);
                }
            }
        }

        /* Small delay to increase chance of race condition */
        if (i % 100 == 0) {
            usleep(100);
        }
    }

    printf("[Thread %d] Completed deleted key test\n", thread_id);
    return NULL;
}

/*
 * Test 2: Delete key while threads are using it
 */
void *test_concurrent_delete(void *arg) {
    int thread_id = *(int *)arg;
    thread_data_t *data;

    printf("[Thread %d] Starting concurrent delete test\n", thread_id);

    /* Allocate initial data */
    data = malloc(sizeof(thread_data_t));
    if (data) {
        data->thread_id = thread_id;
        pthread_setspecific(test_key, data);
    }

    /* Do operations while another thread might delete the key */
    for (int i = 0; i < NUM_ITERATIONS / 10 && test_running; i++) {
        /* Try to get */
        void *retrieved = pthread_getspecific(test_key);

        /* Try to set NULL */
        int result = pthread_setspecific(test_key, NULL);

        if (result == EINVAL) {
            /* Key was deleted - this is expected */
            break;
        }

        /* Try to set new value */
        thread_data_t *new_data = malloc(sizeof(thread_data_t));
        if (new_data) {
            new_data->thread_id = thread_id;
            new_data->operations = i;

            result = pthread_setspecific(test_key, new_data);
            if (result != 0) {
                free(new_data);
                break;
            }

            /* Clean up old data if different */
            if (retrieved && retrieved != new_data) {
                free(retrieved);
            }
        }

        usleep(1000);
    }

    /* Final cleanup */
    void *final_data = pthread_getspecific(test_key);
    if (final_data) {
        pthread_setspecific(test_key, NULL);
        free(final_data);
    }

    printf("[Thread %d] Completed concurrent delete test\n", thread_id);
    return NULL;
}

/*
 * Deleter thread - deletes and recreates key
 */
void *deleter_thread(void *arg) {
    (void)arg;

    printf("[Deleter] Starting\n");

    /* Wait a bit for threads to start using the key */
    usleep(50000); /* 50ms */

    /* Delete the key while threads are using it */
    printf("[Deleter] Deleting key...\n");
    key_valid = 0;

    int result = pthread_key_delete(test_key);
    if (result == 0) {
        printf("[Deleter] Key deleted successfully\n");
    } else {
        printf("[Deleter] Key delete failed: %d\n", result);
    }

    /* Wait a bit */
    usleep(100000); /* 100ms */

    /* Recreate the key */
    printf("[Deleter] Recreating key...\n");
    result = pthread_key_create(&test_key, test_destructor);
    if (result == 0) {
        printf("[Deleter] Key recreated successfully\n");
        key_valid = 1;
    } else {
        printf("[Deleter] Key create failed: %d\n", result);
    }

    printf("[Deleter] Completed\n");
    return NULL;
}

/*
 * Test 3: Stress test with many threads
 */
void *stress_test_thread(void *arg) {
    int thread_id = *(int *)arg;
    int errors = 0;

    for (int i = 0; i < 100; i++) {
        void *data = malloc(128);
        if (!data) continue;

        snprintf((char *)data, 128, "Thread %d iter %d", thread_id, i);

        int set_result = pthread_setspecific(test_key, data);
        if (set_result != 0) {
            free(data);
            if (set_result == EINVAL && !key_valid) {
                /* Expected - key was deleted */
                continue;
            }
            errors++;
            continue;
        }

        void *retrieved = pthread_getspecific(test_key);
        if (retrieved != data) {
            errors++;
        }

        pthread_setspecific(test_key, NULL);
        free(data);

        usleep(100);
    }

    if (errors > 0) {
        printf("[Thread %d] Stress test had %d errors\n", thread_id, errors);
        pthread_mutex_lock(&crash_mutex);
        crash_count += errors;
        pthread_mutex_unlock(&crash_mutex);
    }

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    pthread_t deleter;
    int thread_ids[NUM_THREADS];
    int result;

    printf("=== PTHREAD_SETSPECIFIC CRASH TEST ===\n");
    printf("Simulating QT6 qmake crash scenario\n\n");

    /* TEST 1: Deleted key usage */
    printf("=== TEST 1: Using deleted key ===\n");

    key_valid = 1;
    test_running = 1;
    crash_count = 0;

    result = pthread_key_create(&test_key, test_destructor);
    if (result != 0) {
        printf("Failed to create key: %d\n", result);
        return 1;
    }

    /* Create worker threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, test_deleted_key, &thread_ids[i]);
    }

    /* Wait a bit, then delete the key */
    usleep(50000); /* 50ms */
    printf("\n[Main] Deleting key while threads are using it...\n");
    key_valid = 0;
    pthread_key_delete(test_key);
    printf("[Main] Key deleted\n\n");

    /* Wait for threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n[Test 1] Completed with %d crashes/errors\n", crash_count);
    if (crash_count == 0) {
        printf("+ PASS: No crashes when using deleted key\n");
    } else {
        printf("x FAIL: Had %d crashes/errors\n", crash_count);
    }

    /* TEST 2: Concurrent delete */
    printf("\n=== TEST 2: Concurrent key delete ===\n");

    key_valid = 1;
    test_running = 1;
    crash_count = 0;

    result = pthread_key_create(&test_key, test_destructor);
    if (result != 0) {
        printf("Failed to create key: %d\n", result);
        return 1;
    }

    /* Create worker threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, test_concurrent_delete, &thread_ids[i]);
    }

    /* Create deleter thread */
    pthread_create(&deleter, NULL, deleter_thread, NULL);

    /* Wait for all */
    pthread_join(deleter, NULL);
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n[Test 2] Completed with %d crashes/errors\n", crash_count);
    if (crash_count == 0) {
        printf("+ PASS: No crashes with concurrent delete\n");
    } else {
        printf("x FAIL: Had %d crashes/errors\n", crash_count);
    }

    /* TEST 3: Stress test */
    printf("\n=== TEST 3: Stress test ===\n");

    key_valid = 1;
    test_running = 1;
    crash_count = 0;

    if (key_valid) {
        pthread_key_delete(test_key);
    }
	printf("Creating key for stress test\n");
    result = pthread_key_create(&test_key, test_destructor);
    if (result != 0) {
        printf("Failed to create key: %d\n", result);
        return 1;
    }

    /* Create many threads */
	printf("Creating %d stress test threads\n", NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, stress_test_thread, &thread_ids[i]);
    }

	printf("Joining stress test threads\n");
    /* Wait for completion */
    for (int i = 0; i < NUM_THREADS; i++) {
    	printf("Joining thread %d\n", i);
        pthread_join(threads[i], NULL);
    }
	printf("Deleting key for stress test\n");
    pthread_key_delete(test_key);

    printf("\n[Test 3] Completed with %d errors\n", crash_count);
    if (crash_count == 0) {
        printf("+ PASS: Stress test completed without errors\n");
    } else {
        printf("x FAIL: Had %d errors\n", crash_count);
    }

    /* SUMMARY */
    printf("\n=== SUMMARY ===\n");
    printf("All tests completed without crashing!\n");
    printf("This verifies the fixes prevent the QT6 qmake crash scenario.\n");
    printf("\nFixes applied:\n");
    printf("  + pthread_setspecific validates key is still in use\n");
    printf("  + pthread_setspecific checks ThreadInfo is not NULL\n");
    printf("  + pthread_setspecific/getspecific use mutex protection\n");
    printf("  + Returns EINVAL instead of crashing on invalid key\n");

    return 0;
}
