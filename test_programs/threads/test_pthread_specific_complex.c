/*
 * Complex test for pthread_setspecific/pthread_getspecific
 *
 * This test demonstrates:
 * 1. Thread-specific data with multiple threads
 * 2. Multiple TSD keys
 * 3. Destructor functions being called
 * 4. Using spawnvpe to launch child processes that also use TSD
 * 5. Data isolation between threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

/* Thread-specific data keys */
static pthread_key_t thread_id_key;
static pthread_key_t thread_data_key;
static pthread_key_t counter_key;

/* Statistics */
static volatile int destructor_calls = 0;
static pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Thread-specific data structures */
typedef struct {
    int thread_num;
    char name[64];
    unsigned long magic;
} thread_id_data_t;

typedef struct {
    char buffer[256];
    int operation_count;
    FILE *log_file;
} thread_data_t;

typedef struct {
    unsigned long count;
    unsigned long sum;
} counter_data_t;

/*
 * Destructor function for thread_id_data
 */
void thread_id_destructor(void *data) {
    thread_id_data_t *tid_data = (thread_id_data_t *)data;

    pthread_mutex_lock(&stats_mutex);
    destructor_calls++;
    pthread_mutex_unlock(&stats_mutex);

    if (tid_data) {
        printf("[Destructor] Cleaning up thread_id_data for thread %d (%s)\n",
               tid_data->thread_num, tid_data->name);
        free(tid_data);
    }
}

/*
 * Destructor function for thread_data
 */
void thread_data_destructor(void *data) {
    thread_data_t *tdata = (thread_data_t *)data;

    pthread_mutex_lock(&stats_mutex);
    destructor_calls++;
    pthread_mutex_unlock(&stats_mutex);

    if (tdata) {
        printf("[Destructor] Cleaning up thread_data, operations: %d\n",
               tdata->operation_count);
        if (tdata->log_file) {
            fclose(tdata->log_file);
        }
        free(tdata);
    }
}

/*
 * Destructor function for counter_data
 */
void counter_destructor(void *data) {
    counter_data_t *counter = (counter_data_t *)data;

    pthread_mutex_lock(&stats_mutex);
    destructor_calls++;
    pthread_mutex_unlock(&stats_mutex);

    if (counter) {
        printf("[Destructor] Cleaning up counter, count: %lu, sum: %lu\n",
               counter->count, counter->sum);
        free(counter);
    }
}

/*
 * Initialize thread-specific data for a thread
 */
int init_thread_specific_data(int thread_num, const char *thread_name) {
    /* Allocate and set thread ID data */
    thread_id_data_t *tid_data = malloc(sizeof(thread_id_data_t));
    if (!tid_data) {
        perror("malloc thread_id_data");
        return -1;
    }

    tid_data->thread_num = thread_num;
    snprintf(tid_data->name, sizeof(tid_data->name), "%s", thread_name);
    tid_data->magic = 0xDEADBEEF + thread_num;

    if (pthread_setspecific(thread_id_key, tid_data) != 0) {
        perror("pthread_setspecific thread_id_key");
        free(tid_data);
        return -1;
    }

    /* Allocate and set thread data */
    thread_data_t *tdata = malloc(sizeof(thread_data_t));
    if (!tdata) {
        perror("malloc thread_data");
        return -1;
    }

    memset(tdata->buffer, 0, sizeof(tdata->buffer));
    tdata->operation_count = 0;
    tdata->log_file = NULL;

    if (pthread_setspecific(thread_data_key, tdata) != 0) {
        perror("pthread_setspecific thread_data_key");
        free(tdata);
        return -1;
    }

    /* Allocate and set counter data */
    counter_data_t *counter = malloc(sizeof(counter_data_t));
    if (!counter) {
        perror("malloc counter_data");
        return -1;
    }

    counter->count = 0;
    counter->sum = 0;

    if (pthread_setspecific(counter_key, counter) != 0) {
        perror("pthread_setspecific counter_key");
        free(counter);
        return -1;
    }

    printf("[Thread %d] Initialized TSD: %s\n", thread_num, thread_name);
    return 0;
}

/*
 * Perform operations using thread-specific data
 */
void perform_thread_operations(int iterations) {
    /* Get thread-specific data */
    thread_id_data_t *tid_data = pthread_getspecific(thread_id_key);
    thread_data_t *tdata = pthread_getspecific(thread_data_key);
    counter_data_t *counter = pthread_getspecific(counter_key);

    if (!tid_data || !tdata || !counter) {
        fprintf(stderr, "[ERROR] Failed to get thread-specific data\n");
        return;
    }

    printf("[Thread %d: %s] Starting %d operations\n",
           tid_data->thread_num, tid_data->name, iterations);

    /* Perform some operations */
    for (int i = 0; i < iterations; i++) {
        /* Update buffer */
        snprintf(tdata->buffer, sizeof(tdata->buffer),
                 "Thread %d, iteration %d", tid_data->thread_num, i);
        tdata->operation_count++;

        /* Update counter */
        counter->count++;
        counter->sum += i;

        /* Verify magic number */
        if (tid_data->magic != 0xDEADBEEF + tid_data->thread_num) {
            fprintf(stderr, "[ERROR] Magic number corrupted in thread %d!\n",
                    tid_data->thread_num);
            return;
        }

        /* Small delay to interleave operations */
        if (i % 10 == 0) {
            usleep(1000);
        }
    }

    printf("[Thread %d: %s] Completed: ops=%d, count=%lu, sum=%lu\n",
           tid_data->thread_num, tid_data->name,
           tdata->operation_count, counter->count, counter->sum);
}

/*
 * Worker thread function
 */
void *worker_thread(void *arg) {
    int thread_num = *(int *)arg;
    char thread_name[64];

    snprintf(thread_name, sizeof(thread_name), "Worker-%d", thread_num);

    /* Initialize thread-specific data */
    if (init_thread_specific_data(thread_num, thread_name) != 0) {
        return NULL;
    }

    /* Perform operations */
    perform_thread_operations(50);

    /* Get final data to verify isolation */
    thread_id_data_t *tid_data = pthread_getspecific(thread_id_key);
    thread_data_t *tdata = pthread_getspecific(thread_data_key);

    printf("[Thread %d] Final buffer content: '%s'\n",
           thread_num, tdata ? tdata->buffer : "NULL");

    /* Note: destructors will be called automatically when thread exits */
    return NULL;
}

/*
 * Main function for child process (launched via spawnvpe)
 */
int child_process_main(int process_num) {
    printf("\n=== CHILD PROCESS %d STARTED ===\n", process_num);

    /* Create keys in child process */
    if (pthread_key_create(&thread_id_key, thread_id_destructor) != 0) {
        perror("pthread_key_create thread_id_key in child");
        return 1;
    }

    if (pthread_key_create(&thread_data_key, thread_data_destructor) != 0) {
        perror("pthread_key_create thread_data_key in child");
        return 1;
    }

    if (pthread_key_create(&counter_key, counter_destructor) != 0) {
        perror("pthread_key_create counter_key in child");
        return 1;
    }

    /* Initialize main thread's TSD */
    char main_name[64];
    snprintf(main_name, sizeof(main_name), "ChildProcess-%d-Main", process_num);
    init_thread_specific_data(100 + process_num, main_name);

    /* Create worker threads in child process */
    pthread_t threads[3];
    int thread_nums[3];

    for (int i = 0; i < 3; i++) {
        thread_nums[i] = (process_num * 10) + i;
        if (pthread_create(&threads[i], NULL, worker_thread, &thread_nums[i]) != 0) {
            perror("pthread_create in child");
            continue;
        }
    }

    /* Perform operations in main thread */
    perform_thread_operations(30);

    /* Wait for threads */
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Cleanup keys */
    pthread_key_delete(thread_id_key);
    pthread_key_delete(thread_data_key);
    pthread_key_delete(counter_key);

    printf("=== CHILD PROCESS %d FINISHED ===\n\n", process_num);
    return 0;
}

/*
 * Main function
 */
int main(int argc, char *argv[]) {
    /* Check if we're running as a child process */
    if (argc == 2 && strcmp(argv[1], "--child") == 0) {
        return child_process_main(1);
    }
    if (argc == 3 && strcmp(argv[1], "--child") == 0) {
        int process_num = atoi(argv[2]);
        return child_process_main(process_num);
    }

    printf("=== PTHREAD THREAD-SPECIFIC DATA COMPLEX TEST ===\n\n");

    /* Create keys with destructors */
    printf("Creating TSD keys with destructors...\n");

    if (pthread_key_create(&thread_id_key, thread_id_destructor) != 0) {
        perror("pthread_key_create thread_id_key");
        return 1;
    }

    if (pthread_key_create(&thread_data_key, thread_data_destructor) != 0) {
        perror("pthread_key_create thread_data_key");
        return 1;
    }

    if (pthread_key_create(&counter_key, counter_destructor) != 0) {
        perror("pthread_key_create counter_key");
        return 1;
    }

    printf("TSD keys created successfully\n\n");

    /* Initialize main thread's TSD */
    init_thread_specific_data(0, "MainThread");

    /* Create multiple worker threads */
    printf("Creating worker threads...\n");
    pthread_t threads[5];
    int thread_nums[5];

    for (int i = 0; i < 5; i++) {
        thread_nums[i] = i + 1;
        if (pthread_create(&threads[i], NULL, worker_thread, &thread_nums[i]) != 0) {
            perror("pthread_create");
            continue;
        }
    }

    /* Perform operations in main thread */
    printf("\nMain thread performing operations...\n");
    perform_thread_operations(40);

    /* Wait for all threads to complete */
    printf("\nWaiting for threads to complete...\n");
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nAll worker threads completed\n");

    /* Test data isolation: verify main thread's data is intact */
    thread_id_data_t *main_tid_data = pthread_getspecific(thread_id_key);
    thread_data_t *main_tdata = pthread_getspecific(thread_data_key);
    counter_data_t *main_counter = pthread_getspecific(counter_key);

    printf("\nMain thread final state:\n");
    printf("  Thread ID: %d (%s)\n", main_tid_data->thread_num, main_tid_data->name);
    printf("  Operations: %d\n", main_tdata->operation_count);
    printf("  Counter: %lu, Sum: %lu\n", main_counter->count, main_counter->sum);
    printf("  Buffer: '%s'\n", main_tdata->buffer);

    /* Launch child processes using spawnvpe */
    printf("\n=== LAUNCHING CHILD PROCESSES ===\n");

    char *child_argv[4];
    char child_num_str1[16] = "1";
    char child_num_str2[16] = "2";

    /* Get program path */
    char program_path[1024];
    if (argc > 0) {
        strncpy(program_path, argv[0], sizeof(program_path) - 1);
    } else {
        strncpy(program_path, "./test_pthread_specific_complex", sizeof(program_path) - 1);
    }

    /* Launch first child */
    child_argv[0] = program_path;
    child_argv[1] = "--child";
    child_argv[2] = child_num_str1;
    child_argv[3] = NULL;

    printf("Launching child process 1...\n");
    pid_t child1 = spawnvpe(program_path, (const char **)child_argv, NULL, NULL, -1, -1, -1);
    if (child1 < 0) {
        int saved_errno = errno;
        fprintf(stderr, "Failed to spawn child 1: %s\n", strerror(saved_errno));
    } else {
        printf("Child process 1 spawned with PID: %d\n", child1);
    }

    /* Launch second child */
    child_argv[2] = child_num_str2;

    printf("Launching child process 2...\n");
    pid_t child2 = spawnvpe(program_path, (const char **)child_argv, NULL, NULL, -1, -1, -1);
    if (child2 < 0) {
        int saved_errno = errno;
        fprintf(stderr, "Failed to spawn child 2: %s\n", strerror(saved_errno));
    } else {
        printf("Child process 2 spawned with PID: %d\n", child2);
    }

    /* Wait for child processes */
    if (child1 > 0) {
        int status;
        printf("Waiting for child 1 (PID %d)...\n", child1);
        waitpid(child1, &status, 0);
        printf("Child 1 exited with status: %d\n", WEXITSTATUS(status));
    }

    if (child2 > 0) {
        int status;
        printf("Waiting for child 2 (PID %d)...\n", child2);
        waitpid(child2, &status, 0);
        printf("Child 2 exited with status: %d\n", WEXITSTATUS(status));
    }

    /* Verify main thread data is still intact after spawning children */
    main_tid_data = pthread_getspecific(thread_id_key);
    main_tdata = pthread_getspecific(thread_data_key);

    printf("\n=== VERIFICATION ===\n");
    printf("Main thread data after child processes:\n");
    printf("  Thread ID: %d (%s)\n", main_tid_data->thread_num, main_tid_data->name);
    printf("  Magic: 0x%lX (expected: 0x%lX)\n",
           main_tid_data->magic, 0xDEADBEEF);

    if (main_tid_data->magic == 0xDEADBEEF) {
        printf("  + Data integrity verified!\n");
    } else {
        printf("  x Data corruption detected!\n");
    }

    /* Cleanup keys - this will trigger destructors for main thread's data */
    printf("\nDeleting TSD keys...\n");
    pthread_key_delete(thread_id_key);
    pthread_key_delete(thread_data_key);
    pthread_key_delete(counter_key);

    /* Note: The destructors for main thread's data won't be called by key_delete,
     * they would only be called when the thread exits. Since we're in main,
     * we need to manually free the data or let the process exit. */

    printf("\nTotal destructor calls: %d\n", destructor_calls);
    printf("\n=== TEST COMPLETE ===\n");

    return 0;
}
