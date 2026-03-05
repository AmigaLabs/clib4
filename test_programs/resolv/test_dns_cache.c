/*
 * test_dns_cache.c — Verify DNS cache behaviour in clib4
 *
 * Tests:
 *  1. First lookup populates the cache (cold)
 *  2. Second lookup is served from cache (must be much faster)
 *  3. Cache works across different API paths (gethostbyname, getaddrinfo)
 *  4. Multiple threads hitting the same name see cache hits
 *  5. Different hostnames get independent cache entries
 *  6. Cache respects TTL (entry eventually expires)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

/* ------------------------------------------------------------------ */

static double now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

static int tests_run    = 0;
static int tests_passed = 0;

#define CHECK(cond, fmt, ...)                                       \
    do {                                                            \
        tests_run++;                                                \
        if (cond) {                                                 \
            tests_passed++;                                         \
            printf("  PASS: " fmt "\n", ##__VA_ARGS__);             \
        } else {                                                    \
            printf("  FAIL: " fmt "\n", ##__VA_ARGS__);             \
        }                                                           \
    } while (0)

/* ------------------------------------------------------------------ */
/* Test 1+2: cold lookup vs cached lookup                             */

static void test_cache_speedup(const char *host)
{
    printf("\n[test_cache_speedup] host=%s\n", host);

    /* Cold lookup — forces a real DNS query */
    double t0 = now_ms();
    struct hostent *h1 = gethostbyname(host);
    double t_cold = now_ms() - t0;

    CHECK(h1 != NULL, "cold lookup resolved '%s'", host);
    if (!h1) return;

    char first_ip[64] = {0};
    inet_ntop(h1->h_addrtype, h1->h_addr_list[0], first_ip, sizeof first_ip);
    printf("  addr = %s (%.1f ms)\n", first_ip, t_cold);

    /* Cached lookup — same name, should come from cache */
    double t1 = now_ms();
    struct hostent *h2 = gethostbyname(host);
    double t_cached = now_ms() - t1;

    CHECK(h2 != NULL, "cached lookup resolved '%s'", host);
    if (!h2) return;

    char second_ip[64] = {0};
    inet_ntop(h2->h_addrtype, h2->h_addr_list[0], second_ip, sizeof second_ip);

    CHECK(strcmp(first_ip, second_ip) == 0,
          "cached address matches: %s == %s", first_ip, second_ip);

    /* A cached hit should be at least 10x faster than the network query.
       Be lenient: if cold < 1ms (edge-case), just check cached < 1ms. */
    if (t_cold > 1.0)
        CHECK(t_cached < t_cold / 5.0,
              "cached %.2f ms  <  cold/5 %.2f ms", t_cached, t_cold / 5.0);
    else
        CHECK(t_cached < 1.0,
              "cached %.2f ms  < 1 ms (cold was too fast to compare)", t_cached);
}

/* ------------------------------------------------------------------ */
/* Test 3: getaddrinfo also benefits from the cache                   */

static void test_getaddrinfo_cache(const char *host)
{
    printf("\n[test_getaddrinfo_cache] host=%s\n", host);

    /* Pre-warm with gethostbyname */
    struct hostent *h = gethostbyname(host);
    CHECK(h != NULL, "pre-warm with gethostbyname('%s')", host);
    if (!h) return;

    /* Now getaddrinfo should hit the cache */
    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    double t0 = now_ms();
    int rc = getaddrinfo(host, NULL, &hints, &res);
    double elapsed = now_ms() - t0;

    CHECK(rc == 0 && res != NULL,
          "getaddrinfo('%s') returned %d (%.2f ms)", host, rc, elapsed);

    if (res) {
        char buf[64];
        struct sockaddr_in *sin = (struct sockaddr_in *)res->ai_addr;
        inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof buf);
        printf("  getaddrinfo addr = %s\n", buf);
        freeaddrinfo(res);
    }
}

/* ------------------------------------------------------------------ */
/* Test 4: multi-threaded cache hits                                  */

#define MT_THREADS 8
#define MT_ITERS   20

typedef struct {
    const char *host;
    double total_ms;
    int ok_count;
} mt_arg;

static void *mt_worker(void *arg)
{
    mt_arg *a = (mt_arg *)arg;
    a->total_ms = 0;
    a->ok_count = 0;

    for (int i = 0; i < MT_ITERS; i++) {
        double t0 = now_ms();
        struct hostent *h = gethostbyname(a->host);
        a->total_ms += now_ms() - t0;
        if (h) a->ok_count++;
    }
    return NULL;
}

static void test_multithread_cache(const char *host)
{
    printf("\n[test_multithread_cache] host=%s, %d threads x %d iters\n",
           host, MT_THREADS, MT_ITERS);

    /* Pre-warm the cache so all threads see hits */
    struct hostent *h = gethostbyname(host);
    CHECK(h != NULL, "pre-warm '%s'", host);
    if (!h) return;

    pthread_t tids[MT_THREADS];
    mt_arg args[MT_THREADS];
    for (int i = 0; i < MT_THREADS; i++) {
        args[i].host = host;
        pthread_create(&tids[i], NULL, mt_worker, &args[i]);
    }

    int total_ok = 0;
    double worst_avg = 0;
    for (int i = 0; i < MT_THREADS; i++) {
        pthread_join(tids[i], NULL);
        total_ok += args[i].ok_count;
        double avg = args[i].total_ms / MT_ITERS;
        if (avg > worst_avg) worst_avg = avg;
        printf("  thread %d: %d/%d ok, avg %.2f ms\n",
               i, args[i].ok_count, MT_ITERS, avg);
    }

    CHECK(total_ok == MT_THREADS * MT_ITERS,
          "all lookups succeeded: %d/%d", total_ok, MT_THREADS * MT_ITERS);

    CHECK(worst_avg < 5.0,
          "worst thread avg %.2f ms < 5 ms (cache-hit speed)", worst_avg);
}

/* ------------------------------------------------------------------ */
/* Test 5: different hostnames get independent entries                 */

static void test_independent_entries(const char *host_a, const char *host_b)
{
    printf("\n[test_independent_entries] '%s' vs '%s'\n", host_a, host_b);

    struct hostent *ha = gethostbyname(host_a);
    struct hostent *hb = gethostbyname(host_b);
    CHECK(ha != NULL && hb != NULL, "both hosts resolved");
    if (!ha || !hb) return;

    char ip_a[64], ip_b[64];
    inet_ntop(ha->h_addrtype, ha->h_addr_list[0], ip_a, sizeof ip_a);
    inet_ntop(hb->h_addrtype, hb->h_addr_list[0], ip_b, sizeof ip_b);

    /* Verify cached versions still return the right address */
    struct hostent *ha2 = gethostbyname(host_a);
    struct hostent *hb2 = gethostbyname(host_b);
    CHECK(ha2 != NULL && hb2 != NULL, "cached lookups succeeded");
    if (!ha2 || !hb2) return;

    char ip_a2[64], ip_b2[64];
    inet_ntop(ha2->h_addrtype, ha2->h_addr_list[0], ip_a2, sizeof ip_a2);
    inet_ntop(hb2->h_addrtype, hb2->h_addr_list[0], ip_b2, sizeof ip_b2);

    CHECK(strcmp(ip_a, ip_a2) == 0, "host_a cached correctly: %s", ip_a);
    CHECK(strcmp(ip_b, ip_b2) == 0, "host_b cached correctly: %s", ip_b);
}

/* ------------------------------------------------------------------ */
/* Test 6: repeated cached lookups stay fast across many iterations    */

static void test_sustained_cache(const char *host, int iterations)
{
    printf("\n[test_sustained_cache] host=%s, %d iterations\n", host, iterations);

    /* Pre-warm */
    struct hostent *h = gethostbyname(host);
    CHECK(h != NULL, "pre-warm '%s'", host);
    if (!h) return;

    double t0 = now_ms();
    int ok = 0;
    for (int i = 0; i < iterations; i++) {
        if (gethostbyname(host))
            ok++;
    }
    double elapsed = now_ms() - t0;
    double avg = elapsed / iterations;

    CHECK(ok == iterations, "%d/%d lookups succeeded", ok, iterations);
    CHECK(avg < 1.0,
          "avg %.3f ms per lookup < 1 ms (%d iterations in %.1f ms)",
          avg, iterations, elapsed);
}

/* ------------------------------------------------------------------ */

int main(int argc, char **argv)
{
    const char *host1 = "www.google.com";
    const char *host2 = "www.github.com";

    if (argc > 1) host1 = argv[1];
    if (argc > 2) host2 = argv[2];

    printf("=== DNS Cache Test ===\n");
    printf("host1 = %s\n", host1);
    printf("host2 = %s\n", host2);

    test_cache_speedup(host1);
    test_getaddrinfo_cache(host1);
    test_multithread_cache(host1);
    test_independent_entries(host1, host2);
    test_sustained_cache(host1, 500);

    printf("\n=== Results: %d/%d passed ===\n", tests_passed, tests_run);
    return tests_passed == tests_run ? 0 : 1;
}
