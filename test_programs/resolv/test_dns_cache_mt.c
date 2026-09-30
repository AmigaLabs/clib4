/*
 * test_dns_cache_mt.c — concurrency test for the clib4 DNS cache
 *
 * The cache lives in __clib4->dns_cache and is guarded by dns_cache_lock:
 * __dns_cache_lookup() takes it shared, __dns_cache_store() takes it
 * exclusively. Several readers therefore run through the cache at the same
 * time, which is only safe as long as the lookup really does not write to it.
 *
 * The cache is internal, so everything here goes through getaddrinfo().
 *
 *  Phase 1 (readers only, warm cache)
 *      Repeated lookups of the same name must return byte-identical results.
 *      Nothing writes to the cache during this phase, so every thread copies
 *      the same entry: any difference in the address list means a reader saw
 *      the entry in a state nobody put it in.
 *
 *  Phase 2 (readers and writers together)
 *      Writers resolve a long list of unrelated hostnames, which miss, go out
 *      to the network and come back through __dns_cache_store(). With
 *      DNS_CACHE_SIZE at 64 and 4-way probing, a list this long keeps
 *      evicting entries while the readers are still reading them.
 *
 *      The invariant is cross contamination: a lookup of host A must never
 *      come back with an address belonging to host B. A torn read -- a reader
 *      copying an entry while a store overwrites that same slot -- shows up
 *      exactly like this.
 *
 *  Phase 3 (same name, both families)
 *      family is part of the cache key, so these are two separate entries
 *      being read at the same time.
 *
 *  Phase 4 (case folding)
 *      DNS names are case insensitive and the cache folds them, so every
 *      spelling of a name must land on the same entry rather than starting a
 *      fresh query and a fresh slot.
 *
 * Round-robin hosts change their address set between queries, so "the address
 * is not the one recorded at startup" is not treated as an error -- it is
 * counted and printed. Only an address belonging to the *other* host is a
 * failure.
 *
 * Note what this test can and cannot say: passing means no race was observed
 * in these runs, not that none exists. Widening the window (more writers, more
 * iterations) buys more confidence. Stores cannot be counted from outside the
 * library, so the writer figures below are lookups, not stores.
 *
 * Needs working DNS. With no network the test reports SKIP rather than FAIL.
 *
 * Usage: test_dns_cache_mt [hostA] [hostB] [readers] [writers] [iterations]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_SET       32
#define ADDRSTR       INET6_ADDRSTRLEN
#define MAX_THREADS   16

/*
 * Names for the writers to churn through. They only have to be distinct and
 * resolvable: each one that misses produces a store, and enough of them keep
 * the 64 slot cache turning over. Ones that do not resolve are just counted
 * and skipped.
 */
static const char *const churn_hosts[] = {
    "www.wikipedia.org",   "www.cloudflare.com",  "www.mozilla.org",
    "www.debian.org",      "www.kernel.org",      "www.python.org",
    "www.gnu.org",         "www.archlinux.org",   "www.ietf.org",
    "www.iana.org",        "ftp.gnu.org",         "www.php.net",
    "www.perl.org",        "www.ruby-lang.org",   "www.postgresql.org",
    "www.freebsd.org",     "www.netbsd.org",      "www.openbsd.org",
    "www.gnome.org",       "www.kde.org",         "www.videolan.org",
    "www.ffmpeg.org",      "www.libsdl.org",      "www.sqlite.org",
};
#define NCHURN ((int) (sizeof churn_hosts / sizeof churn_hosts[0]))

/* ------------------------------------------------------------------ */

static int tests_run;
static int tests_passed;

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

static double now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

/* ------------------------------------------------------------------ */
/* An address set, as text, in the order getaddrinfo() returned it.     */

struct addrset {
    char a[MAX_SET][ADDRSTR];
    int n;
};

static int resolve_set(const char *host, int family, struct addrset *out)
{
    struct addrinfo hints, *res = NULL, *ai;
    int rc;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;

    out->n = 0;

    rc = getaddrinfo(host, NULL, &hints, &res);
    if (rc != 0 || res == NULL)
        return rc ? rc : EAI_FAIL;

    for (ai = res; ai != NULL && out->n < MAX_SET; ai = ai->ai_next) {
        const void *src;

        if (ai->ai_family == AF_INET)
            src = &((struct sockaddr_in *) ai->ai_addr)->sin_addr;
        else if (ai->ai_family == AF_INET6)
            src = &((struct sockaddr_in6 *) ai->ai_addr)->sin6_addr;
        else
            continue;

        if (inet_ntop(ai->ai_family, src, out->a[out->n], ADDRSTR) != NULL)
            out->n++;
    }

    freeaddrinfo(res);

    return out->n > 0 ? 0 : EAI_FAIL;
}

static int set_contains(const struct addrset *s, const char *addr)
{
    for (int i = 0; i < s->n; i++)
        if (strcmp(s->a[i], addr) == 0)
            return 1;
    return 0;
}

static int set_identical(const struct addrset *x, const struct addrset *y)
{
    if (x->n != y->n)
        return 0;
    for (int i = 0; i < x->n; i++)
        if (strcmp(x->a[i], y->a[i]) != 0)
            return 0;
    return 1;
}

static void set_print(const char *label, const struct addrset *s)
{
    printf("  %s:", label);
    for (int i = 0; i < s->n; i++)
        printf(" %s", s->a[i]);
    printf("\n");
}

/* Flip the case of the letters selected by the bits of 'variant'. */
static void case_variant(const char *host, unsigned variant, char *out, size_t outsz)
{
    size_t i, bit = 0;

    for (i = 0; i + 1 < outsz && host[i] != '\0'; i++) {
        unsigned char c = (unsigned char) host[i];

        if (isalpha(c)) {
            out[i] = (variant >> (bit % 16)) & 1 ? (char) toupper(c) : (char) tolower(c);
            bit++;
        } else {
            out[i] = (char) c;
        }
    }
    out[i] = '\0';
}

/* ------------------------------------------------------------------ */

struct reader_arg {
    const char *host;
    int family;
    int iterations;
    const struct addrset *expect;   /* phase 1/3: exact match required */
    const struct addrset *foreign;  /* phase 2: must never show up     */
    int lookups;
    int failures;
    int unstable;                   /* differs from expect             */
    int refreshed;                  /* not in own set: round-robin     */
    int crossovers;                 /* saw the other host's address    */
};

/* ------------------------------------------------------------------ */
/* Phase 1: readers only, warm cache, results must not vary            */

static void *stable_reader(void *p)
{
    struct reader_arg *a = (struct reader_arg *) p;

    for (int i = 0; i < a->iterations; i++) {
        struct addrset got;

        if (resolve_set(a->host, a->family, &got) != 0) {
            a->failures++;
            continue;
        }
        a->lookups++;

        if (!set_identical(&got, a->expect))
            a->unstable++;
    }
    return NULL;
}

static void test_stable_reads(const char *host, int family,
                              int nthreads, int iterations)
{
    pthread_t tid[MAX_THREADS];
    struct reader_arg arg[MAX_THREADS];
    struct addrset warm;
    int unstable = 0, failures = 0, lookups = 0;

    printf("\n[phase 1] %d readers x %d lookups of '%s', warm cache\n",
           nthreads, iterations, host);

    if (resolve_set(host, family, &warm) != 0) {
        printf("  SKIP: '%s' does not resolve\n", host);
        return;
    }
    /* Second call so we are certainly reading a cached entry. */
    if (resolve_set(host, family, &warm) != 0) {
        printf("  SKIP: '%s' stopped resolving\n", host);
        return;
    }
    set_print("cached entry", &warm);

    for (int i = 0; i < nthreads; i++) {
        memset(&arg[i], 0, sizeof arg[i]);
        arg[i].host = host;
        arg[i].family = family;
        arg[i].iterations = iterations;
        arg[i].expect = &warm;
        if (pthread_create(&tid[i], NULL, stable_reader, &arg[i]) != 0) {
            printf("  FAIL: pthread_create failed for reader %d\n", i);
            nthreads = i;
            break;
        }
    }

    for (int i = 0; i < nthreads; i++) {
        pthread_join(tid[i], NULL);
        lookups  += arg[i].lookups;
        failures += arg[i].failures;
        unstable += arg[i].unstable;
    }

    printf("  %d lookups, %d errors, %d differing results\n",
           lookups, failures, unstable);

    CHECK(lookups > 0, "readers completed lookups (%d)", lookups);

    /*
     * Nothing stores during this phase, so every reader copies the same
     * entry. Any variation means a reader observed the entry mid-change.
     * A TTL expiry could refresh it legitimately, but the minimum TTL the
     * cache enforces is 10 s and this phase is far shorter than that.
     */
    CHECK(unstable == 0,
          "cached reads are identical across threads (%d differing)", unstable);
}

/* ------------------------------------------------------------------ */
/* Phase 2: readers and writers together                               */

static volatile int stop_readers;

static void *churn_reader(void *p)
{
    struct reader_arg *a = (struct reader_arg *) p;

    while (!stop_readers) {
        struct addrset got;

        if (resolve_set(a->host, a->family, &got) != 0) {
            a->failures++;
            continue;
        }
        a->lookups++;

        for (int i = 0; i < got.n; i++) {
            if (set_contains(a->foreign, got.a[i]))
                a->crossovers++;
            else if (!set_contains(a->expect, got.a[i]))
                a->refreshed++;
        }
    }
    return NULL;
}

struct writer_arg {
    int family;
    int rounds;
    int start;                      /* offset into churn_hosts[]       */
    int lookups;
    int failures;
};

static void *churn_writer(void *p)
{
    struct writer_arg *a = (struct writer_arg *) p;

    /*
     * Each name that is not currently cached goes out to the network and
     * comes back through __dns_cache_store() -- the exclusive-lock traffic
     * the readers have to survive. Once the cache is full every miss also
     * evicts whatever else was in the probed slots.
     */
    for (int r = 0; r < a->rounds; r++) {
        for (int i = 0; i < NCHURN; i++) {
            struct addrset got;
            const char *host = churn_hosts[(a->start + i) % NCHURN];

            if (resolve_set(host, a->family, &got) != 0)
                a->failures++;
            else
                a->lookups++;
        }
    }
    return NULL;
}

static void test_readers_vs_writers(const char *host_a, const char *host_b,
                                    int family, int nreaders, int nwriters,
                                    int rounds)
{
    pthread_t rtid[MAX_THREADS], wtid[MAX_THREADS];
    struct reader_arg rarg[MAX_THREADS];
    struct writer_arg warg[MAX_THREADS];
    struct addrset set_a, set_b;
    int lookups = 0, writes = 0, failures = 0;
    int crossovers = 0, refreshed = 0;
    double t0;

    printf("\n[phase 2] %d readers + %d writers (%d rounds of %d names)\n",
           nreaders, nwriters, rounds, NCHURN);
    printf("          A='%s'  B='%s'\n", host_a, host_b);

    if (resolve_set(host_a, family, &set_a) != 0) {
        printf("  SKIP: '%s' does not resolve\n", host_a);
        return;
    }
    if (resolve_set(host_b, family, &set_b) != 0) {
        printf("  SKIP: '%s' does not resolve\n", host_b);
        return;
    }
    set_print("A", &set_a);
    set_print("B", &set_b);

    /*
     * Two hosts that already share an address give us nothing to detect
     * contamination with.
     */
    for (int i = 0; i < set_a.n; i++) {
        if (set_contains(&set_b, set_a.a[i])) {
            printf("  SKIP: '%s' and '%s' share address %s;"
                   " pick two unrelated hosts\n", host_a, host_b, set_a.a[i]);
            return;
        }
    }

    stop_readers = 0;
    t0 = now_ms();

    for (int i = 0; i < nreaders; i++) {
        int on_a = (i % 2) == 0;

        memset(&rarg[i], 0, sizeof rarg[i]);
        rarg[i].host    = on_a ? host_a : host_b;
        rarg[i].family  = family;
        rarg[i].expect  = on_a ? &set_a : &set_b;
        rarg[i].foreign = on_a ? &set_b : &set_a;
        if (pthread_create(&rtid[i], NULL, churn_reader, &rarg[i]) != 0) {
            printf("  FAIL: pthread_create failed for reader %d\n", i);
            nreaders = i;
            break;
        }
    }

    for (int i = 0; i < nwriters; i++) {
        memset(&warg[i], 0, sizeof warg[i]);
        warg[i].family = family;
        warg[i].rounds = rounds;
        /* Spread the writers over the list so they are not in lockstep. */
        warg[i].start  = (i * NCHURN) / (nwriters > 0 ? nwriters : 1);
        if (pthread_create(&wtid[i], NULL, churn_writer, &warg[i]) != 0) {
            printf("  FAIL: pthread_create failed for writer %d\n", i);
            nwriters = i;
            break;
        }
    }

    /* Readers run until the writers are done. */
    for (int i = 0; i < nwriters; i++) {
        pthread_join(wtid[i], NULL);
        writes   += warg[i].lookups;
        failures += warg[i].failures;
    }

    stop_readers = 1;

    for (int i = 0; i < nreaders; i++) {
        pthread_join(rtid[i], NULL);
        lookups    += rarg[i].lookups;
        failures   += rarg[i].failures;
        crossovers += rarg[i].crossovers;
        refreshed  += rarg[i].refreshed;
    }

    printf("  %.0f ms: %d reader lookups, %d writer lookups, %d errors\n",
           now_ms() - t0, lookups, writes, failures);
    printf("  %d addresses differed from the startup set (round-robin or"
           " TTL refresh, not an error)\n", refreshed);

    CHECK(lookups > 0, "readers ran during the churn (%d lookups)", lookups);
    CHECK(writes > 0, "writers churned the cache (%d lookups)", writes);

    /*
     * The one that matters: an address that belongs to the other host can
     * only come out of an entry that was read while it was being replaced.
     */
    CHECK(crossovers == 0,
          "no lookup returned the other host's address (%d crossovers)",
          crossovers);
}

/* ------------------------------------------------------------------ */
/* Phase 3: the same name under both families at once                  */

static void test_family_keys(const char *host, int nthreads, int iterations)
{
    pthread_t tid[MAX_THREADS];
    struct reader_arg arg[MAX_THREADS];
    struct addrset v4, v6;
    int lookups = 0, failures = 0, unstable = 0;

    printf("\n[phase 3] '%s' read as AF_INET and AF_INET6 at the same time\n",
           host);

    if (resolve_set(host, AF_INET, &v4) != 0) {
        printf("  SKIP: no IPv4 answer for '%s'\n", host);
        return;
    }
    if (resolve_set(host, AF_INET6, &v6) != 0) {
        printf("  SKIP: no IPv6 answer for '%s'\n", host);
        return;
    }
    /* Make sure both entries are in the cache before the threads start. */
    (void) resolve_set(host, AF_INET, &v4);
    (void) resolve_set(host, AF_INET6, &v6);
    set_print("v4", &v4);
    set_print("v6", &v6);

    for (int i = 0; i < nthreads; i++) {
        int v4_thread = (i % 2) == 0;

        memset(&arg[i], 0, sizeof arg[i]);
        arg[i].host       = host;
        arg[i].family     = v4_thread ? AF_INET : AF_INET6;
        arg[i].iterations = iterations;
        arg[i].expect     = v4_thread ? &v4 : &v6;
        if (pthread_create(&tid[i], NULL, stable_reader, &arg[i]) != 0) {
            nthreads = i;
            break;
        }
    }

    for (int i = 0; i < nthreads; i++) {
        pthread_join(tid[i], NULL);
        lookups  += arg[i].lookups;
        failures += arg[i].failures;
        unstable += arg[i].unstable;
    }

    printf("  %d lookups, %d errors, %d differing results\n",
           lookups, failures, unstable);

    CHECK(unstable == 0,
          "v4 and v6 entries stayed separate (%d differing)", unstable);
}

/* ------------------------------------------------------------------ */
/* Phase 4: case folding — every spelling is the same cache entry      */

static void test_case_folding(const char *host, int family)
{
    struct addrset warm, got;
    char name[256];
    int checked = 0, mismatches = 0, failures = 0;
    double slowest = 0.0;

    printf("\n[phase 4] '%s' looked up with different spellings\n", host);

    if (resolve_set(host, family, &warm) != 0) {
        printf("  SKIP: '%s' does not resolve\n", host);
        return;
    }
    /* Second call so the entry is certainly cached. */
    if (resolve_set(host, family, &warm) != 0) {
        printf("  SKIP: '%s' stopped resolving\n", host);
        return;
    }
    set_print("cached entry", &warm);

    for (unsigned v = 1; v <= 12; v++) {
        double t0;

        case_variant(host, v, name, sizeof name);
        if (strcmp(name, host) == 0)
            continue;

        t0 = now_ms();
        if (resolve_set(name, family, &got) != 0) {
            failures++;
            continue;
        }
        double elapsed = now_ms() - t0;
        if (elapsed > slowest)
            slowest = elapsed;

        checked++;
        if (!set_identical(&got, &warm)) {
            mismatches++;
            printf("  differs: %s -> %s\n", name, got.n ? got.a[0] : "(none)");
        }
    }

    printf("  %d spellings checked, %d errors, slowest %.2f ms\n",
           checked, failures, slowest);

    CHECK(checked > 0, "alternative spellings were resolvable (%d)", checked);

    /*
     * Folded keys mean every spelling hits the entry that is already there.
     * Without folding each one would be a fresh query, and for a round-robin
     * host the answer would usually come back in a different order.
     */
    CHECK(mismatches == 0,
          "every spelling returned the same cached entry (%d differing)",
          mismatches);

    /* Informational: a served-from-cache answer is not a network round trip. */
    printf("  %s\n", slowest < 2.0
           ? "  (all spellings answered at cache speed)"
           : "  (note: some spellings took long enough to look like queries)");
}

/* ------------------------------------------------------------------ */

int main(int argc, char **argv)
{
    const char *host_a  = "www.google.com";
    const char *host_b  = "www.github.com";
    int readers    = 8;
    int writers    = 2;
    int iterations = 200;
    int rounds     = 2;

    if (argc > 1) host_a = argv[1];
    if (argc > 2) host_b = argv[2];
    if (argc > 3) readers = atoi(argv[3]);
    if (argc > 4) writers = atoi(argv[4]);
    if (argc > 5) iterations = atoi(argv[5]);

    if (readers < 1) readers = 1;
    if (readers > MAX_THREADS) readers = MAX_THREADS;
    if (writers < 1) writers = 1;
    if (writers > MAX_THREADS) writers = MAX_THREADS;
    if (iterations < 1) iterations = 1;

    printf("=== DNS cache concurrency test ===\n");
    printf("A=%s  B=%s  readers=%d  writers=%d  iterations=%d\n",
           host_a, host_b, readers, writers, iterations);

    test_stable_reads(host_a, AF_INET, readers, iterations);
    test_readers_vs_writers(host_a, host_b, AF_INET, readers, writers, rounds);
    test_family_keys(host_a, readers, iterations);
    test_case_folding(host_a, AF_INET);

    printf("\n=== Results: %d/%d passed ===\n", tests_passed, tests_run);

    if (tests_run == 0) {
        printf("Nothing ran -- DNS is not available here.\n");
        return 77;   /* conventional "skipped" exit status */
    }

    return tests_passed == tests_run ? 0 : 1;
}
