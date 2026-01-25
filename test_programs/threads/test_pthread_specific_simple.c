/*
 * Simple test for pthread_setspecific/pthread_getspecific
 *
 * This test verifies:
 * 1. Basic TSD key creation and deletion
 * 2. Setting and getting thread-specific data
 * 3. Data isolation between threads
 * 4. Destructor function being called
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

static pthread_key_t data_key;
static volatile int destructor_count = 0;

typedef struct {
    int thread_id;
    int counter;
    char message[128];
} thread_data_t;

/*
 * Destructor function - called when thread exits
 */
void data_destructor(void *data) {
    thread_data_t *tdata = (thread_data_t *)data;

    if (tdata) {
        printf("[Destructor] Thread %d: counter=%d, message='%s'\n",
               tdata->thread_id, tdata->counter, tdata->message);
        destructor_count++;
        free(tdata);
    }
}

/*
 * Worker thread function
 */
void *worker_thread(void *arg) {
    int thread_id = *(int *)arg;

    printf("[Thread %d] Started\n", thread_id);

    /* Allocate thread-specific data */
    thread_data_t *data = malloc(sizeof(thread_data_t));
    if (!data) {
        perror("malloc");
        return NULL;
    }

    data->thread_id = thread_id;
    data->counter = 0;
    snprintf(data->message, sizeof(data->message), "Hello from thread %d", thread_id);

    /* Set thread-specific data */
    if (pthread_setspecific(data_key, data) != 0) {
        perror("pthread_setspecific");
        free(data);
        return NULL;
    }

    printf("[Thread %d] Set TSD: message='%s'\n", thread_id, data->message);

    /* Perform some operations using thread-specific data */
    for (int i = 0; i < 10; i++) {
        /* Get thread-specific data */
        thread_data_t *retrieved = pthread_getspecific(data_key);

        if (!retrieved) {
            fprintf(stderr, "[Thread %d] ERROR: Failed to get TSD\n", thread_id);
            return NULL;
        }

        /* Verify it's our data */
        if (retrieved->thread_id != thread_id) {
            fprintf(stderr, "[Thread %d] ERROR: Got wrong thread's data! (got %d)\n",
                    thread_id, retrieved->thread_id);
            return NULL;
        }

        /* Update counter */
        retrieved->counter++;

        usleep(10000); /* 10ms */
    }

    /* Get final data */
    thread_data_t *final_data = pthread_getspecific(data_key);
    printf("[Thread %d] Final counter: %d\n", thread_id, final_data->counter);

    /* Destructor will be called automatically when thread exits */
    return NULL;
}

int main(void) {
    printf("=== PTHREAD THREAD-SPECIFIC DATA SIMPLE TEST ===\n\n");

    /* Create key with destructor */
    printf("Creating TSD key with destructor...\n");
    if (pthread_key_create(&data_key, data_destructor) != 0) {
        perror("pthread_key_create");
        return 1;
    }
    printf("Key created successfully\n\n");

    /* Test with main thread */
    printf("Testing with main thread...\n");
    thread_data_t *main_data = malloc(sizeof(thread_data_t));
    main_data->thread_id = 0;
    main_data->counter = 100;
    strcpy(main_data->message, "Main thread data");

    if (pthread_setspecific(data_key, main_data) != 0) {
        perror("pthread_setspecific main");
        return 1;
    }

    thread_data_t *retrieved = pthread_getspecific(data_key);
    printf("Main thread: thread_id=%d, counter=%d, message='%s'\n",
           retrieved->thread_id, retrieved->counter, retrieved->message);
    printf("+ Main thread TSD works\n\n");

    /* Create worker threads */
    printf("Creating 3 worker threads...\n");
    pthread_t threads[3];
    int thread_ids[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, worker_thread, &thread_ids[i]) != 0) {
            perror("pthread_create");
            continue;
        }
    }

    /* Verify main thread's data is still intact */
    retrieved = pthread_getspecific(data_key);
    printf("\nMain thread data while workers run:\n");
    printf("  thread_id=%d, counter=%d, message='%s'\n",
           retrieved->thread_id, retrieved->counter, retrieved->message);

    if (retrieved->thread_id == 0 && retrieved->counter == 100) {
        printf("+ Main thread data isolation verified\n");
    } else {
        printf("x ERROR: Main thread data was corrupted!\n");
    }

    /* Wait for threads */
    printf("\nWaiting for worker threads to complete...\n");
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nAll threads completed\n");
    printf("Destructors called: %d (expected: 3)\n", destructor_count);

    if (destructor_count == 3) {
        printf("+ All destructors called correctly\n");
    } else {
        printf("x ERROR: Expected 3 destructor calls, got %d\n", destructor_count);
    }

    /* Cleanup */
    /* Note: Main thread's destructor won't be called by pthread_key_delete,
     * only when thread exits. We manually free it here. */
    printf("\nManually freeing main thread's data...\n");
    free(main_data);

    printf("Deleting TSD key...\n");
    if (pthread_key_delete(data_key) != 0) {
        perror("pthread_key_delete");
        return 1;
    }

    printf("\n=== TEST COMPLETE ===\n");
    return 0;
}
