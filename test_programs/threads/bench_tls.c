#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NKEYS PTHREAD_KEYS_MAX
#define NITER 10000000UL

// Timing helper
static uint64_t get_time_ns(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int main(void) {
    // Warm up and check
    for (size_t i = 0; i < NKEYS; ++i)
    	pthread_setspecific(i, (void*)(uintptr_t)i);

    // Benchmark new TLS accessor
    uint64_t start_new = get_time_ns();
    for (size_t iter = 0; iter < NITER; ++iter) {
    	for (size_t i = 0; i < NKEYS; ++i) {
    		pthread_setspecific(i, (void*)(uintptr_t)iter);
    		(void)pthread_getspecific(i);
    	}
    }
    uint64_t end_new = get_time_ns();

    double time_new = (end_new - start_new) / 1e9;

    printf("TLS Benchmark:\n");
    printf("TLS accessors: %.3f seconds\n", time_new);

    return 0;
}