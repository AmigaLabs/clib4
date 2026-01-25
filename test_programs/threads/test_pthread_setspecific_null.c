/*
 * Test pthread_setspecific with NULL value
 *
 * Verifies POSIX behavior of pthread_setspecific(key, NULL):
 * - Setting NULL disassociates the value from the key
 * - pthread_getspecific returns NULL after setspecific(key, NULL)
 * - The destructor is NOT called for NULL values
 * - Allows manual cleanup before thread termination
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static pthread_key_t test_key;
static volatile int destructor_call_count = 0;
static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Destructor - should be called ONLY for non-NULL values
 */
void test_destructor(void *data) {
    pthread_mutex_lock(&count_mutex);
    destructor_call_count++;
    pthread_mutex_unlock(&count_mutex);

    printf("[Destructor] Called for data: %p (value: %d)\n",
           data, data ? *(int *)data : -1);

    if (data) {
        free(data);
    }
}

/*
 * Test 1: Thread that explicitly sets NULL
 * The destructor should NOT be called
 */
void *test_null_explicit(void *arg) {
    int thread_num = *(int *)arg;
    printf("\n[Thread %d] Test: Set value then NULL\n", thread_num);

    /* Allocate and set a value */
    int *data = malloc(sizeof(int));
    *data = thread_num * 100;

    pthread_setspecific(test_key, data);
    printf("[Thread %d] Set value: %d\n", thread_num, *data);

    /* Verify it's set */
    int *retrieved = pthread_getspecific(test_key);
    printf("[Thread %d] Retrieved: %p (value: %d)\n",
           thread_num, retrieved, retrieved ? *retrieved : -1);

    /* Now set NULL - this disassociates the value */
    printf("[Thread %d] Setting NULL to disassociate value\n", thread_num);
    pthread_setspecific(test_key, NULL);

    /* Verify it's now NULL */
    retrieved = pthread_getspecific(test_key);
    printf("[Thread %d] After NULL: %p\n", thread_num, retrieved);

    if (retrieved == NULL) {
        printf("[Thread %d] + PASS: getspecific returns NULL after setspecific(NULL)\n",
               thread_num);
    } else {
        printf("[Thread %d] x FAIL: Expected NULL, got %p\n", thread_num, retrieved);
    }

    /* Manual cleanup of previous data */
    free(data);
    printf("[Thread %d] Manually freed data (destructor should NOT be called)\n",
           thread_num);

    return NULL;
}

/*
 * Test 2: Thread that keeps the non-NULL value
 * The destructor SHOULD be called
 */
void *test_with_value(void *arg) {
    int thread_num = *(int *)arg;
    printf("\n[Thread %d] Test: Keep value set\n", thread_num);

    /* Allocate and set a value */
    int *data = malloc(sizeof(int));
    *data = thread_num * 200;

    pthread_setspecific(test_key, data);
    printf("[Thread %d] Set value: %d\n", thread_num, *data);

    /* Verify it's set */
    int *retrieved = pthread_getspecific(test_key);
    printf("[Thread %d] Retrieved: %p (value: %d)\n",
           thread_num, retrieved, retrieved ? *retrieved : -1);

    printf("[Thread %d] Exiting with value set (destructor SHOULD be called)\n",
           thread_num);

    return NULL;
}

/*
 * Test 3: Thread that never sets any value
 * The destructor should NOT be called
 */
void *test_never_set(void *arg) {
    int thread_num = *(int *)arg;
    printf("\n[Thread %d] Test: Never set any value\n", thread_num);

    /* Verify it's NULL by default */
    void *retrieved = pthread_getspecific(test_key);
    printf("[Thread %d] Initial value: %p\n", thread_num, retrieved);

    if (retrieved == NULL) {
        printf("[Thread %d] + PASS: Initial value is NULL\n", thread_num);
    } else {
        printf("[Thread %d] x FAIL: Expected NULL, got %p\n", thread_num, retrieved);
    }

    printf("[Thread %d] Exiting without setting value (destructor should NOT be called)\n",
           thread_num);

    return NULL;
}

/*
 * Test 4: Thread that sets and resets NULL multiple times
 */
void *test_multiple_null(void *arg) {
    int thread_num = *(int *)arg;
    printf("\n[Thread %d] Test: Multiple set/NULL cycles\n", thread_num);

    for (int i = 0; i < 3; i++) {
        /* Allocate and set */
        int *data = malloc(sizeof(int));
        *data = thread_num * 1000 + i;
        pthread_setspecific(test_key, data);
        printf("[Thread %d] Cycle %d: Set value %d\n", thread_num, i, *data);

        /* Reset to NULL and free manually */
        pthread_setspecific(test_key, NULL);
        free(data);
        printf("[Thread %d] Cycle %d: Set NULL and freed\n", thread_num, i);

        /* Verify it's NULL */
        void *check = pthread_getspecific(test_key);
        if (check != NULL) {
            printf("[Thread %d] x FAIL: Expected NULL after cycle %d\n", thread_num, i);
        }
    }

    printf("[Thread %d] + PASS: Multiple NULL cycles completed\n", thread_num);

    return NULL;
}

int main(void) {
    printf("=== PTHREAD_SETSPECIFIC NULL VALUE TEST ===\n");
    printf("\nPOSIX Behavior:\n");
    printf("  - pthread_setspecific(key, NULL) disassociates the value\n");
    printf("  - Destructor is NOT called for NULL values\n");
    printf("  - Allows manual cleanup before thread termination\n");

    /* Create the key with destructor */
    printf("\nCreating TSD key with destructor...\n");
    if (pthread_key_create(&test_key, test_destructor) != 0) {
        perror("pthread_key_create");
        return 1;
    }
    printf("Key created successfully\n");

    /* Test 1: Set NULL explicitly */
    printf("\n=== TEST 1: Explicit NULL ===\n");
    pthread_t t1;
    int num1 = 1;
    pthread_create(&t1, NULL, test_null_explicit, &num1);
    pthread_join(t1, NULL);

    usleep(100000); /* 100ms to give destructors time */

    int count_after_t1 = destructor_call_count;
    printf("\n[Main] Destructor calls after Test 1: %d (expected: 0)\n", count_after_t1);
    if (count_after_t1 == 0) {
        printf("+ PASS: No destructor called for NULL value\n");
    } else {
        printf("x FAIL: Destructor was called %d times for NULL value\n", count_after_t1);
    }

    /* Test 2: Keep the value */
    printf("\n=== TEST 2: Keep Value Set ===\n");
    pthread_t t2;
    int num2 = 2;
    pthread_create(&t2, NULL, test_with_value, &num2);
    pthread_join(t2, NULL);

    usleep(100000);

    int count_after_t2 = destructor_call_count;
    printf("\n[Main] Destructor calls after Test 2: %d (expected: 1)\n", count_after_t2);
    if (count_after_t2 == 1) {
        printf("+ PASS: Destructor called once for non-NULL value\n");
    } else {
        printf("x FAIL: Expected 1 destructor call, got %d\n", count_after_t2);
    }

    /* Test 3: Never set a value */
    printf("\n=== TEST 3: Never Set ===\n");
    pthread_t t3;
    int num3 = 3;
    pthread_create(&t3, NULL, test_never_set, &num3);
    pthread_join(t3, NULL);

    usleep(100000);

    int count_after_t3 = destructor_call_count;
    printf("\n[Main] Destructor calls after Test 3: %d (expected: 1, no change)\n",
           count_after_t3);
    if (count_after_t3 == 1) {
        printf("+ PASS: No destructor called when never set\n");
    } else {
        printf("x FAIL: Unexpected destructor count: %d\n", count_after_t3);
    }

    /* Test 4: Multiple NULL cycles */
    printf("\n=== TEST 4: Multiple NULL Cycles ===\n");
    pthread_t t4;
    int num4 = 4;
    pthread_create(&t4, NULL, test_multiple_null, &num4);
    pthread_join(t4, NULL);

    usleep(100000);

    int count_after_t4 = destructor_call_count;
    printf("\n[Main] Destructor calls after Test 4: %d (expected: 1, no change)\n",
           count_after_t4);
    if (count_after_t4 == 1) {
        printf("+ PASS: No destructors called for multiple NULL sets\n");
    } else {
        printf("x FAIL: Unexpected destructor count: %d\n", count_after_t4);
    }

    /* Cleanup */
    pthread_key_delete(test_key);

    /* Summary */
    printf("\n=== SUMMARY ===\n");
    printf("Total destructor calls: %d (expected: 1)\n", destructor_call_count);
    printf("\nTests completed:\n");
    printf("  Test 1 (Explicit NULL):    %s\n",
           count_after_t1 == 0 ? "PASS" : "FAIL");
    printf("  Test 2 (Keep Value):       %s\n",
           count_after_t2 == 1 ? "PASS" : "FAIL");
    printf("  Test 3 (Never Set):        %s\n",
           count_after_t3 == 1 ? "PASS" : "FAIL");
    printf("  Test 4 (Multiple NULL):    %s\n",
           count_after_t4 == 1 ? "PASS" : "FAIL");

    if (destructor_call_count == 1) {
        printf("\n+ ALL TESTS PASSED!\n");
        printf("\nConclusion:\n");
        printf("  + pthread_setspecific(key, NULL) works correctly\n");
        printf("  + Destructor is NOT called for NULL values\n");
        printf("  + Manual cleanup is possible before thread exit\n");
        return 0;
    } else {
        printf("\nx SOME TESTS FAILED!\n");
        return 1;
    }
}
