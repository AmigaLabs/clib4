/*
 * getaddrinfo_threads.c
 *
 * Tests getaddrinfo called from 10 concurrent threads in two modes:
 *
 *  - Mode A (locked):   each thread acquires a mutex before calling
 *                        getaddrinfo, serialising all lookups.  This is
 *                        the safe approach for implementations that are
 *                        NOT reentrant.
 *
 *  - Mode B (unlocked): each thread calls getaddrinfo concurrently with
 *                        no lock.  This is safe because our implementation
 *                        uses gethostbyname_r / getservbyname_r internally.
 *
 * Both modes must produce identical results.  Elapsed time is printed so
 * the concurrency gain of Mode B is visible.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

#define NUM_THREADS  10
#define NUM_HOSTS    5

/* Hosts each thread will resolve (one per thread iteration). */
static const char *hosts[NUM_HOSTS] = {
    "www.google.com",
    "www.github.com",
    "www.cloudflare.com",
    "www.amazon.com",
    "www.wikipedia.org",
};

/* ------------------------------------------------------------------ */
/* Shared mutex used only in Mode A                                     */
/* ------------------------------------------------------------------ */
static pthread_mutex_t getaddrinfo_lock = PTHREAD_MUTEX_INITIALIZER;

/* ------------------------------------------------------------------ */
/* Per-thread result storage                                            */
/* ------------------------------------------------------------------ */
typedef struct {
    int    thread_id;
    int    use_lock;      /* 1 = Mode A, 0 = Mode B */
    int    errors;
    int    lookups;
} thread_arg_t;

/* ------------------------------------------------------------------ */
/* Thread worker                                                        */
/* ------------------------------------------------------------------ */
static void *
worker(void *arg) {
    thread_arg_t *ta = (thread_arg_t *) arg;
    char addr_str[INET6_ADDRSTRLEN];
    int i;

    for (i = 0; i < NUM_HOSTS; i++) {
        const char *host = hosts[i % NUM_HOSTS];
        struct addrinfo hints, *res = NULL;
        int ret;

        memset(&hints, 0, sizeof hints);
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if (ta->use_lock)
            pthread_mutex_lock(&getaddrinfo_lock);

        ret = getaddrinfo(host, "http", &hints, &res);

        if (ta->use_lock)
            pthread_mutex_unlock(&getaddrinfo_lock);

        if (ret != 0) {
            fprintf(stderr, "[thread %2d] getaddrinfo(%s) failed: %s\n",
                    ta->thread_id, host, gai_strerror(ret));
            ta->errors++;
            continue;
        }

        /* Extract the first address for display. */
        void *addr_ptr = NULL;
        if (res->ai_family == AF_INET) {
            addr_ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
        } else if (res->ai_family == AF_INET6) {
            addr_ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
        }

        if (addr_ptr)
            inet_ntop(res->ai_family, addr_ptr, addr_str, sizeof addr_str);
        else
            snprintf(addr_str, sizeof addr_str, "<unknown family %d>", res->ai_family);

        printf("[thread %2d] %-24s -> %s\n", ta->thread_id, host, addr_str);

        freeaddrinfo(res);
        ta->lookups++;
    }

    return NULL;
}

/* ------------------------------------------------------------------ */
/* Run one mode, return elapsed seconds                                 */
/* ------------------------------------------------------------------ */
static double
run_mode(int use_lock) {
    pthread_t       tids[NUM_THREADS];
    thread_arg_t    args[NUM_THREADS];
    struct timespec t0, t1;
    int i, total_errors = 0, total_lookups = 0;

    printf("\n=== Mode %s (%d threads x %d lookups each) ===\n",
           use_lock ? "A - serialised with mutex" : "B - fully concurrent",
           NUM_THREADS, NUM_HOSTS);

    clock_gettime(CLOCK_MONOTONIC, &t0);

    for (i = 0; i < NUM_THREADS; i++) {
        args[i].thread_id = i;
        args[i].use_lock  = use_lock;
        args[i].errors    = 0;
        args[i].lookups   = 0;
        if (pthread_create(&tids[i], NULL, worker, &args[i]) != 0) {
            fprintf(stderr, "pthread_create failed for thread %d: %s\n",
                    i, strerror(errno));
        }
    }

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(tids[i], NULL);
        total_errors  += args[i].errors;
        total_lookups += args[i].lookups;
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);

    double elapsed = (t1.tv_sec - t0.tv_sec) +
                     (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("--- Mode %s: %d/%d lookups succeeded, %.3f s elapsed ---\n",
           use_lock ? "A" : "B",
           total_lookups, NUM_THREADS * NUM_HOSTS, elapsed);

    return elapsed;
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */
int
main(void) {
    double elapsed_a, elapsed_b;

    printf("getaddrinfo thread-safety test\n");
    printf("Resolving %d hosts from %d threads\n", NUM_HOSTS, NUM_THREADS);

    elapsed_a = run_mode(1); /* serialised */
    elapsed_b = run_mode(0); /* concurrent */

    printf("\n=== Summary ===\n");
    printf("Mode A (mutex):      %.3f s\n", elapsed_a);
    printf("Mode B (concurrent): %.3f s\n", elapsed_b);
    if (elapsed_a > 0.001)
        printf("Speedup:             %.2fx\n", elapsed_a / elapsed_b);

    return 0;
}
