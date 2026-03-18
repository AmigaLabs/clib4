/*
 * pthread_kill test suite for AmigaOS4 / clib4
 *
 * Tests various aspects of pthread_kill behaviour, documenting both the
 * expected POSIX semantics and the AmigaOS4-specific constraints.
 *
 * Compile:
 *   ppc-amigaos-gcc -mcrt=clib4 -std=c11 pthread_kill.c -o pthread_kill -lpthread
 */

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* --------------------------------------------------------------------------
 * Simple pass/fail accounting
 * -------------------------------------------------------------------------- */
static int tests_passed = 0;
static int tests_failed = 0;

static void check(const char *desc, int condition) {
    if (condition) {
        printf("  PASS: %s\n", desc);
        tests_passed++;
    } else {
        printf("  FAIL: %s\n", desc);
        tests_failed++;
    }
}

/* --------------------------------------------------------------------------
 * Shared state for signal-reception tests
 * -------------------------------------------------------------------------- */
static volatile int sigusr1_received = 0;
static volatile int sigusr2_received = 0;

static void handler_sigusr1(int sig) {
    (void)sig;
    sigusr1_received = 1;
}

static void handler_sigusr2(int sig) {
    (void)sig;
    sigusr2_received = 1;
}

/* --------------------------------------------------------------------------
 * Thread helpers
 * -------------------------------------------------------------------------- */

/* A thread that simply sleeps for a long time.  Used as a live target. */
static void *sleeper_thread(void *arg) {
    (void)arg;
    sleep(60);
    return NULL;
}

/* A thread that loops briefly then exits cleanly. */
static void *quick_thread(void *arg) {
    (void)arg;
    usleep(50000); /* 50 ms */
    return NULL;
}

/* A thread that installs SIGUSR1/SIGUSR2 handlers and then sleeps. */
static void *signal_thread(void *arg) {
    (void)arg;
    signal(SIGUSR1, handler_sigusr1);
    signal(SIGUSR2, handler_sigusr2);
    /* Sleep long enough for the main thread to send a signal */
    sleep(60);
    return NULL;
}

/* --------------------------------------------------------------------------
 * Test 1 – sig == 0: valid thread existence check
 * -------------------------------------------------------------------------- */
static void test_sig0_valid_thread(void) {
    puts("\n[Test 1] sig=0 on a live thread (existence check)");
    pthread_t tid;
    pthread_create(&tid, NULL, sleeper_thread, NULL);

    int rc = pthread_kill(tid, 0);
    check("pthread_kill(live_thread, 0) returns 0", rc == 0);

    /* Clean up: terminate the sleeper */
    pthread_cancel(tid);
    pthread_join(tid, NULL);
}

/* --------------------------------------------------------------------------
 * Test 2 – sig == 0: non-existent thread should return ESRCH
 * -------------------------------------------------------------------------- */
static void test_sig0_invalid_thread(void) {
    puts("\n[Test 2] sig=0 on a non-existent thread (should return ESRCH)");

    pthread_t tid;
    pthread_create(&tid, NULL, quick_thread, NULL);
    pthread_join(tid, NULL); /* wait until it is fully gone */

    int rc = pthread_kill(tid, 0);
    check("pthread_kill(dead_thread, 0) returns ESRCH", rc == ESRCH);
}

/* --------------------------------------------------------------------------
 * Test 3 – invalid signal number should return EINVAL
 * -------------------------------------------------------------------------- */
static void test_invalid_signal(void) {
    puts("\n[Test 3] invalid signal number (should return EINVAL)");

    pthread_t tid;
    pthread_create(&tid, NULL, sleeper_thread, NULL);

    int rc = pthread_kill(tid, -1);
    check("pthread_kill(thread, -1) returns EINVAL", rc == EINVAL);

    rc = pthread_kill(tid, NSIG);
    check("pthread_kill(thread, NSIG) returns EINVAL", rc == EINVAL);

    rc = pthread_kill(tid, 999);
    check("pthread_kill(thread, 999) returns EINVAL", rc == EINVAL);

    pthread_cancel(tid);
    pthread_join(tid, NULL);
}

/* --------------------------------------------------------------------------
 * Test 4 – SIGTERM to a live thread
 *
 * On AmigaOS4 SIGTERM is mapped to SIGBREAKF_CTRL_C.  The target thread
 * will not necessarily exit immediately because AmigaOS4 lacks hardware-
 * enforced signal delivery, but the call itself must succeed (return 0).
 * -------------------------------------------------------------------------- */
static void test_sigterm(void) {
    puts("\n[Test 4] SIGTERM to a live thread (AmigaOS4: mapped to CTRL-C)");

    pthread_t tid;
    pthread_create(&tid, NULL, sleeper_thread, NULL);

    int rc = pthread_kill(tid, SIGTERM);
    check("pthread_kill(thread, SIGTERM) returns 0", rc == 0);

    /*
     * The thread may or may not have acted on the signal yet; give it a
     * moment then clean up regardless.
     */
    usleep(100000);
    pthread_cancel(tid);
    pthread_join(tid, NULL);
}

/* --------------------------------------------------------------------------
 * Test 5 – SIGKILL to a live thread
 * -------------------------------------------------------------------------- */
static void test_sigkill(void) {
    puts("\n[Test 5] SIGKILL to a live thread (AmigaOS4: mapped to CTRL-C)");

    pthread_t tid;
    pthread_create(&tid, NULL, sleeper_thread, NULL);

    int rc = pthread_kill(tid, SIGKILL);
    check("pthread_kill(thread, SIGKILL) returns 0", rc == 0);

    usleep(100000);
    pthread_cancel(tid);
    pthread_join(tid, NULL);
}

/* --------------------------------------------------------------------------
 * Test 6 – SIGINT to a live thread
 * -------------------------------------------------------------------------- */
static void test_sigint(void) {
    puts("\n[Test 6] SIGINT to a live thread (AmigaOS4: mapped to CTRL-C)");

    pthread_t tid;
    pthread_create(&tid, NULL, sleeper_thread, NULL);

    int rc = pthread_kill(tid, SIGINT);
    check("pthread_kill(thread, SIGINT) returns 0", rc == 0);

    usleep(100000);
    pthread_cancel(tid);
    pthread_join(tid, NULL);
}

/* --------------------------------------------------------------------------
 * Test 7 – sending a signal to self (pthread_kill on the calling thread)
 *
 * SIGUSR1 is used with a custom handler so that the signal is handled
 * rather than invoking the default termination behaviour.
 * -------------------------------------------------------------------------- */
static volatile int self_sigusr1 = 0;
static void handler_self(int sig) {
    (void)sig;
    self_sigusr1 = 1;
}

static void test_signal_to_self(void) {
    puts("\n[Test 7] pthread_kill to self with SIGUSR1");

    signal(SIGUSR1, handler_self);
    self_sigusr1 = 0;

    int rc = pthread_kill(pthread_self(), SIGUSR1);
    /*
     * On AmigaOS4, signal delivery to self goes through the raise() path.
     * The return value must be 0.
     */
    check("pthread_kill(self, SIGUSR1) returns 0", rc == 0);

    /* The handler may or may not fire synchronously on this platform */
    usleep(10000);
    printf("  INFO: self_sigusr1 = %d (1 means handler fired)\n", self_sigusr1);

    signal(SIGUSR1, SIG_DFL);
}

/* --------------------------------------------------------------------------
 * Test 8 – SIGUSR1/SIGUSR2 with a custom handler in the target thread
 *
 * On a POSIX system the handler would run inside the target thread.  On
 * AmigaOS4 the best we can do is mark the signal as pending and wake the
 * thread.  The test verifies that the call returns 0 (no error).
 * -------------------------------------------------------------------------- */
static void test_sigusr_custom_handler(void) {
    puts("\n[Test 8] SIGUSR1 to a thread with a custom handler");

    sigusr1_received = 0;

    pthread_t tid;
    pthread_create(&tid, NULL, signal_thread, NULL);

    /* Give the thread time to install its handler */
    usleep(100000);

    int rc = pthread_kill(tid, SIGUSR1);
    check("pthread_kill(thread, SIGUSR1) with custom handler returns 0", rc == 0);

    usleep(200000);
    printf("  INFO: sigusr1_received = %d (best-effort delivery on AmigaOS4)\n",
           sigusr1_received);

    pthread_cancel(tid);
    pthread_join(tid, NULL);
}

/* --------------------------------------------------------------------------
 * Test 9 – SIGUSR2 with SIG_IGN in the target thread
 *
 * When the handler is SIG_IGN, pthread_kill should silently succeed (0).
 * -------------------------------------------------------------------------- */
static void *ignore_thread(void *arg) {
    (void)arg;
    signal(SIGUSR2, SIG_IGN);
    sleep(60);
    return NULL;
}

static void test_sigusr_ignored(void) {
    puts("\n[Test 9] SIGUSR2 to a thread with SIG_IGN (should return 0)");

    pthread_t tid;
    pthread_create(&tid, NULL, ignore_thread, NULL);

    usleep(100000);

    int rc = pthread_kill(tid, SIGUSR2);
    check("pthread_kill(thread, SIGUSR2) with SIG_IGN returns 0", rc == 0);

    pthread_cancel(tid);
    pthread_join(tid, NULL);
}

/* --------------------------------------------------------------------------
 * Test 10 – send signal to thread that has already been joined
 *           (should return ESRCH)
 * -------------------------------------------------------------------------- */
static void test_signal_after_join(void) {
    puts("\n[Test 10] pthread_kill after thread has been joined (should return ESRCH)");

    pthread_t tid;
    pthread_create(&tid, NULL, quick_thread, NULL);
    pthread_join(tid, NULL);

    int rc = pthread_kill(tid, SIGUSR1);
    check("pthread_kill on joined thread returns ESRCH", rc == ESRCH);
}

/* --------------------------------------------------------------------------
 * main
 * -------------------------------------------------------------------------- */
int main(void) {
    puts("=== pthread_kill test suite ===");
    puts("Note: AmigaOS4 has no memory protection; POSIX signal delivery");
    puts("to other threads is best-effort (see comments in each test).");

    test_sig0_valid_thread();
    test_sig0_invalid_thread();
    test_invalid_signal();
    test_sigterm();
    test_sigkill();
    test_sigint();
    test_signal_to_self();
    test_sigusr_custom_handler();
    test_sigusr_ignored();
    test_signal_after_join();

    printf("\n=== Results: %d passed, %d failed ===\n",
           tests_passed, tests_failed);

    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
