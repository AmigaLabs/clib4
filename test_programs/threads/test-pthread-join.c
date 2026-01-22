/*
 * Test program to verify pthread_join behavior on AmigaOS/clib4
 * Simulates the async pattern used in run-command-amiga.c
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

/* Simulate the async structure */
struct test_async {
	pthread_t tid;
	void *data;
	volatile int thread_result;  /* AmigaOS workaround */
};

/* Thread function that simulates sideband_demux behavior */
static void *test_thread_func(void *arg)
{
	struct test_async *async = (struct test_async *)arg;
	int test_value = (int)(intptr_t)async->data;
	
	printf("[Thread] Started with test_value=%d\n", test_value);
	
	/* Simulate some work */
	sleep(1);
	
	/* Save result in struct (AmigaOS workaround) */
	async->thread_result = test_value;
	
	printf("[Thread] Exiting with return value=%d (saved in thread_result)\n", test_value);
	
	return (void *)(intptr_t)test_value;
}

/* Test pthread_join with different return values */
static int test_pthread_join_method(int test_value)
{
	struct test_async async;
	void *thread_ret = (void *)(intptr_t)(-999);  /* Initialize to sentinel */
	int join_result;
	
	printf("\n=== Test with value=%d (pthread_join method) ===\n", test_value);
	
	async.data = (void *)(intptr_t)test_value;
	async.thread_result = -1;  /* Initialize to error */
	
	/* Create thread */
	if (pthread_create(&async.tid, NULL, test_thread_func, &async)) {
		fprintf(stderr, "pthread_create failed\n");
		return -1;
	}
	
	printf("[Main] Thread created, TID=%lu\n", (unsigned long)async.tid);
	
	/* Wait for thread */
	printf("[Main] Calling pthread_join...\n");
	join_result = pthread_join(async.tid, &thread_ret);
	
	printf("[Main] pthread_join returned %d\n", join_result);
	printf("[Main] thread_ret pointer value = %p\n", thread_ret);
	printf("[Main] thread_ret as int = %d\n", (int)(intptr_t)thread_ret);
	printf("[Main] async.thread_result = %d\n", async.thread_result);
	
	if (join_result != 0) {
		fprintf(stderr, "[Main] ERROR: pthread_join failed with %d\n", join_result);
		return -1;
	}
	
	/* Check if pthread_join returned the correct value */
	int ret_via_join = (int)(intptr_t)thread_ret;
	int ret_via_struct = async.thread_result;
	
	printf("\n[Result] Expected: %d\n", test_value);
	printf("[Result] Via pthread_join: %d %s\n", 
	       ret_via_join, 
	       (ret_via_join == test_value) ? "+ CORRECT" : "x WRONG");
	printf("[Result] Via struct field: %d %s\n", 
	       ret_via_struct,
	       (ret_via_struct == test_value) ? "+ CORRECT" : "x WRONG");
	
	return (ret_via_join == test_value) ? 0 : -1;
}

int main(int argc, char **argv)
{
	int failures = 0;
	
	printf("Testing pthread_join behavior on AmigaOS/clib4\n");
	printf("==============================================\n");
	
	/* Test with return value 0 (success case) */
	if (test_pthread_join_method(0) != 0)
		failures++;
	
	/* Test with return value -1 (SIDEBAND_REMOTE_ERROR) */
	if (test_pthread_join_method(-1) != 0)
		failures++;
	
	/* Test with return value -2 (SIDEBAND_PROTOCOL_ERROR) */
	if (test_pthread_join_method(-2) != 0)
		failures++;
	
	/* Test with return value 1 (SIDEBAND_PRIMARY) */
	if (test_pthread_join_method(1) != 0)
		failures++;
	
	/* Test with return value 42 (random positive) */
	if (test_pthread_join_method(42) != 0)
		failures++;
	
	printf("\n==============================================\n");
	if (failures == 0) {
		printf("+ ALL TESTS PASSED\n");
		printf("pthread_join correctly returns thread exit values.\n");
	} else {
		printf("x %d TEST(S) FAILED\n", failures);
		printf("pthread_join does NOT correctly return thread exit values.\n");
		printf("RECOMMENDATION: Use async->thread_result workaround.\n");
	}
	
	return failures;
}
