/*
 * Test for pthread_rwlock_timedrdlock() and pthread_rwlock_timedwrlock()
 *
 * Tests:
 *  1. timedrdlock succeeds immediately on unlocked rwlock
 *  2. timedwrlock succeeds immediately on unlocked rwlock
 *  3. timedrdlock succeeds when rwlock is read-locked (shared)
 *  4. timedwrlock times out when rwlock is read-locked
 *  5. timedrdlock times out when rwlock is write-locked
 *  6. timedwrlock times out when rwlock is write-locked
 *  7. timedrdlock succeeds after writer releases lock
 *  8. timedwrlock succeeds after reader releases lock
 *  9. Multiple timed readers concurrently
 * 10. NULL abstime falls back to blocking lock
 * 11. Invalid abstime returns EINVAL
 * 12. Repeated timedrdlock calls (timer reuse stress)
 * 13. Repeated timedwrlock calls (timer reuse stress)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

static int pass_count = 0;
static int fail_count = 0;
static int test_total = 0;

#define PASS(msg) do { test_total++; pass_count++; printf("  + PASS: %s\n", msg); } while(0)
#define FAIL(msg, expected, got) do { test_total++; fail_count++; \
    printf("  x FAIL: %s (expected %d, got %d)\n", msg, expected, got); } while(0)

static void make_abstime(struct timespec *ts, int ms) {
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_nsec += (long)ms * 1000000L;
    while (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

static long elapsed_ms(const struct timespec *start) {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return (now.tv_sec - start->tv_sec) * 1000 + (now.tv_nsec - start->tv_nsec) / 1000000;
}

/* ---- Thread helpers ---- */

typedef struct {
    pthread_rwlock_t *lock;
    int hold_ms;          /* how long to hold the lock */
    int result;           /* return value from lock operation */
    int timeout_ms;       /* for timed operations */
    int use_write;        /* 1=wrlock, 0=rdlock */
} ThreadArg;

static void *hold_rdlock_thread(void *arg) {
    ThreadArg *ta = (ThreadArg *)arg;
    pthread_rwlock_rdlock(ta->lock);
    usleep(ta->hold_ms * 1000);
    pthread_rwlock_unlock(ta->lock);
    return NULL;
}

static void *hold_wrlock_thread(void *arg) {
    ThreadArg *ta = (ThreadArg *)arg;
    pthread_rwlock_wrlock(ta->lock);
    usleep(ta->hold_ms * 1000);
    pthread_rwlock_unlock(ta->lock);
    return NULL;
}

static void *timed_rdlock_thread(void *arg) {
    ThreadArg *ta = (ThreadArg *)arg;
    struct timespec ts;
    make_abstime(&ts, ta->timeout_ms);
    ta->result = pthread_rwlock_timedrdlock(ta->lock, &ts);
    if (ta->result == 0)
        pthread_rwlock_unlock(ta->lock);
    return NULL;
}

static void *timed_wrlock_thread(void *arg) {
    ThreadArg *ta = (ThreadArg *)arg;
    struct timespec ts;
    make_abstime(&ts, ta->timeout_ms);
    ta->result = pthread_rwlock_timedwrlock(ta->lock, &ts);
    if (ta->result == 0)
        pthread_rwlock_unlock(ta->lock);
    return NULL;
}

/* ================================================================== */

static void test1_timedrdlock_unlocked(void) {
    printf("\n--- Test 1: timedrdlock on unlocked rwlock ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
    struct timespec ts;
    make_abstime(&ts, 1000);

    int ret = pthread_rwlock_timedrdlock(&rw, &ts);
    if (ret == 0) PASS("timedrdlock on unlocked returns 0");
    else FAIL("timedrdlock on unlocked returns 0", 0, ret);

    if (ret == 0) pthread_rwlock_unlock(&rw);
    pthread_rwlock_destroy(&rw);
}

static void test2_timedwrlock_unlocked(void) {
    printf("\n--- Test 2: timedwrlock on unlocked rwlock ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
    struct timespec ts;
    make_abstime(&ts, 1000);

    int ret = pthread_rwlock_timedwrlock(&rw, &ts);
    if (ret == 0) PASS("timedwrlock on unlocked returns 0");
    else FAIL("timedwrlock on unlocked returns 0", 0, ret);

    if (ret == 0) pthread_rwlock_unlock(&rw);
    pthread_rwlock_destroy(&rw);
}

static void test3_timedrdlock_while_rdlocked(void) {
    printf("\n--- Test 3: timedrdlock while read-locked (shared OK) ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;

    /* Take a read lock in main thread */
    pthread_rwlock_rdlock(&rw);

    /* Another thread should be able to get a read lock too */
    ThreadArg ta = { .lock = &rw, .timeout_ms = 500, .result = -1 };
    pthread_t t;
    pthread_create(&t, NULL, timed_rdlock_thread, &ta);
    pthread_join(t, NULL);

    if (ta.result == 0) PASS("timedrdlock succeeds while read-locked");
    else FAIL("timedrdlock succeeds while read-locked", 0, ta.result);

    pthread_rwlock_unlock(&rw);
    pthread_rwlock_destroy(&rw);
}

static void test4_timedwrlock_timeout_rdlocked(void) {
    printf("\n--- Test 4: timedwrlock times out while read-locked ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
    struct timespec start;

    /* Hold read lock for 500ms */
    ThreadArg holder = { .lock = &rw, .hold_ms = 500 };
    pthread_t ht;
    pthread_create(&ht, NULL, hold_rdlock_thread, &holder);
    usleep(50000); /* let reader acquire */

    clock_gettime(CLOCK_REALTIME, &start);
    struct timespec ts;
    make_abstime(&ts, 100); /* 100ms timeout — should expire before holder releases */

    int ret = pthread_rwlock_timedwrlock(&rw, &ts);
    long ms = elapsed_ms(&start);
    printf("  timedwrlock returned %d after %ld ms\n", ret, ms);

    if (ret == ETIMEDOUT) PASS("timedwrlock returns ETIMEDOUT");
    else FAIL("timedwrlock returns ETIMEDOUT", ETIMEDOUT, ret);

    if (ms >= 80 && ms <= 300) PASS("timeout elapsed ~100ms");
    else { test_total++; fail_count++; printf("  x FAIL: timeout elapsed ~100ms (got %ld ms)\n", ms); }

    pthread_join(ht, NULL);
    pthread_rwlock_destroy(&rw);
}

static void test5_timedrdlock_timeout_wrlocked(void) {
    printf("\n--- Test 5: timedrdlock times out while write-locked ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
    struct timespec start;

    /* Hold write lock for 500ms */
    ThreadArg holder = { .lock = &rw, .hold_ms = 500 };
    pthread_t ht;
    pthread_create(&ht, NULL, hold_wrlock_thread, &holder);
    usleep(50000);

    clock_gettime(CLOCK_REALTIME, &start);
    struct timespec ts;
    make_abstime(&ts, 100);

    int ret = pthread_rwlock_timedrdlock(&rw, &ts);
    long ms = elapsed_ms(&start);
    printf("  timedrdlock returned %d after %ld ms\n", ret, ms);

    if (ret == ETIMEDOUT) PASS("timedrdlock returns ETIMEDOUT");
    else FAIL("timedrdlock returns ETIMEDOUT", ETIMEDOUT, ret);

    if (ms >= 80 && ms <= 300) PASS("timeout elapsed ~100ms");
    else { test_total++; fail_count++; printf("  x FAIL: timeout elapsed ~100ms (got %ld ms)\n", ms); }

    pthread_join(ht, NULL);
    pthread_rwlock_destroy(&rw);
}

static void test6_timedwrlock_timeout_wrlocked(void) {
    printf("\n--- Test 6: timedwrlock times out while write-locked ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
    struct timespec start;

    ThreadArg holder = { .lock = &rw, .hold_ms = 500 };
    pthread_t ht;
    pthread_create(&ht, NULL, hold_wrlock_thread, &holder);
    usleep(50000);

    clock_gettime(CLOCK_REALTIME, &start);
    struct timespec ts;
    make_abstime(&ts, 100);

    int ret = pthread_rwlock_timedwrlock(&rw, &ts);
    long ms = elapsed_ms(&start);
    printf("  timedwrlock returned %d after %ld ms\n", ret, ms);

    if (ret == ETIMEDOUT) PASS("timedwrlock returns ETIMEDOUT");
    else FAIL("timedwrlock returns ETIMEDOUT", ETIMEDOUT, ret);

    if (ms >= 80 && ms <= 300) PASS("timeout elapsed ~100ms");
    else { test_total++; fail_count++; printf("  x FAIL: timeout elapsed ~100ms (got %ld ms)\n", ms); }

    pthread_join(ht, NULL);
    pthread_rwlock_destroy(&rw);
}

static void test7_timedrdlock_succeeds_after_writer(void) {
    printf("\n--- Test 7: timedrdlock succeeds after writer releases ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;

    /* Hold write lock for 100ms */
    ThreadArg holder = { .lock = &rw, .hold_ms = 100 };
    pthread_t ht;
    pthread_create(&ht, NULL, hold_wrlock_thread, &holder);
    usleep(20000);

    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    struct timespec ts;
    make_abstime(&ts, 500); /* 500ms timeout — should succeed after ~100ms */

    int ret = pthread_rwlock_timedrdlock(&rw, &ts);
    long ms = elapsed_ms(&start);
    printf("  timedrdlock returned %d after %ld ms\n", ret, ms);

    if (ret == 0) PASS("timedrdlock succeeds after writer releases");
    else FAIL("timedrdlock succeeds after writer releases", 0, ret);

    if (ret == 0) pthread_rwlock_unlock(&rw);
    pthread_join(ht, NULL);
    pthread_rwlock_destroy(&rw);
}

static void test8_timedwrlock_succeeds_after_reader(void) {
    printf("\n--- Test 8: timedwrlock succeeds after reader releases ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;

    /* Hold read lock for 100ms */
    ThreadArg holder = { .lock = &rw, .hold_ms = 100 };
    pthread_t ht;
    pthread_create(&ht, NULL, hold_rdlock_thread, &holder);
    usleep(20000);

    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    struct timespec ts;
    make_abstime(&ts, 500);

    int ret = pthread_rwlock_timedwrlock(&rw, &ts);
    long ms = elapsed_ms(&start);
    printf("  timedwrlock returned %d after %ld ms\n", ret, ms);

    if (ret == 0) PASS("timedwrlock succeeds after reader releases");
    else FAIL("timedwrlock succeeds after reader releases", 0, ret);

    if (ret == 0) pthread_rwlock_unlock(&rw);
    pthread_join(ht, NULL);
    pthread_rwlock_destroy(&rw);
}

static void test9_multiple_timed_readers(void) {
    printf("\n--- Test 9: Multiple timed readers concurrently ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
    #define NREADERS 5
    pthread_t threads[NREADERS];
    ThreadArg args[NREADERS];

    for (int i = 0; i < NREADERS; i++) {
        args[i] = (ThreadArg){ .lock = &rw, .timeout_ms = 1000, .result = -1 };
        pthread_create(&threads[i], NULL, timed_rdlock_thread, &args[i]);
    }
    for (int i = 0; i < NREADERS; i++)
        pthread_join(threads[i], NULL);

    int ok = 1;
    for (int i = 0; i < NREADERS; i++) {
        if (args[i].result != 0) { ok = 0; break; }
    }
    if (ok) PASS("all 5 timed readers acquired lock concurrently");
    else { test_total++; fail_count++; printf("  x FAIL: not all readers succeeded\n"); }

    pthread_rwlock_destroy(&rw);
    #undef NREADERS
}

static void test10_null_abstime(void) {
    printf("\n--- Test 10: NULL abstime falls back to blocking lock ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;

    int ret = pthread_rwlock_timedrdlock(&rw, NULL);
    if (ret == 0) PASS("timedrdlock(NULL) rdlock succeeds");
    else FAIL("timedrdlock(NULL) rdlock succeeds", 0, ret);
    if (ret == 0) pthread_rwlock_unlock(&rw);

    ret = pthread_rwlock_timedwrlock(&rw, NULL);
    if (ret == 0) PASS("timedwrlock(NULL) wrlock succeeds");
    else FAIL("timedwrlock(NULL) wrlock succeeds", 0, ret);
    if (ret == 0) pthread_rwlock_unlock(&rw);

    pthread_rwlock_destroy(&rw);
}

static void test11_invalid_abstime(void) {
    printf("\n--- Test 11: Invalid abstime returns EINVAL ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;

    struct timespec bad_ts = { .tv_sec = 0, .tv_nsec = -1 };
    int ret = pthread_rwlock_timedrdlock(&rw, &bad_ts);
    if (ret == EINVAL) PASS("timedrdlock with tv_nsec=-1 returns EINVAL");
    else FAIL("timedrdlock with tv_nsec=-1 returns EINVAL", EINVAL, ret);
    if (ret == 0) pthread_rwlock_unlock(&rw);

    bad_ts.tv_nsec = 1000000000L; /* exactly 1 second in nsec = invalid */
    ret = pthread_rwlock_timedwrlock(&rw, &bad_ts);
    if (ret == EINVAL) PASS("timedwrlock with tv_nsec=1e9 returns EINVAL");
    else FAIL("timedwrlock with tv_nsec=1e9 returns EINVAL", EINVAL, ret);
    if (ret == 0) pthread_rwlock_unlock(&rw);

    pthread_rwlock_destroy(&rw);
}

static void test12_repeated_timedrdlock(void) {
    printf("\n--- Test 12: Repeated timedrdlock (timer reuse x20) ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
    int failures = 0;

    for (int i = 0; i < 20; i++) {
        struct timespec ts;
        make_abstime(&ts, 200);
        int ret = pthread_rwlock_timedrdlock(&rw, &ts);
        if (ret != 0) {
            failures++;
            printf("  iter %d: FAILED (ret=%d)\n", i, ret);
        } else {
            pthread_rwlock_unlock(&rw);
        }
    }
    if (failures == 0) PASS("20 repeated timedrdlock all succeeded");
    else { test_total++; fail_count++; printf("  x FAIL: %d/20 iterations failed\n", failures); }

    pthread_rwlock_destroy(&rw);
}

static void test13_repeated_timedwrlock(void) {
    printf("\n--- Test 13: Repeated timedwrlock (timer reuse x20) ---\n");
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
    int failures = 0;

    for (int i = 0; i < 20; i++) {
        struct timespec ts;
        make_abstime(&ts, 200);
        int ret = pthread_rwlock_timedwrlock(&rw, &ts);
        if (ret != 0) {
            failures++;
            printf("  iter %d: FAILED (ret=%d)\n", i, ret);
        } else {
            pthread_rwlock_unlock(&rw);
        }
    }
    if (failures == 0) PASS("20 repeated timedwrlock all succeeded");
    else { test_total++; fail_count++; printf("  x FAIL: %d/20 iterations failed\n", failures); }

    pthread_rwlock_destroy(&rw);
}

int main(void) {
    printf("==========================================================\n");
    printf(" pthread_rwlock_timedrdlock / timedwrlock test\n");
    printf("==========================================================\n");

    test1_timedrdlock_unlocked();
    test2_timedwrlock_unlocked();
    test3_timedrdlock_while_rdlocked();
    test4_timedwrlock_timeout_rdlocked();
    test5_timedrdlock_timeout_wrlocked();
    test6_timedwrlock_timeout_wrlocked();
    test7_timedrdlock_succeeds_after_writer();
    test8_timedwrlock_succeeds_after_reader();
    test9_multiple_timed_readers();
    test10_null_abstime();
    test11_invalid_abstime();
    test12_repeated_timedrdlock();
    test13_repeated_timedwrlock();

    printf("\n==========================================================\n");
    printf(" Summary: %d/%d passed, %d failed\n", pass_count, test_total, fail_count);
    printf("==========================================================\n");

    return fail_count > 0 ? 1 : 0;
}
