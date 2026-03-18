/*
 * pthread_kill test suite for AmigaOS4 / clib4
 *
 * Tests 8 and 9 specifically exercise the task exception hook added in
 * StarterFunc: even a thread that never calls Wait() or any blocking function
 * can receive a signal because AmigaOS4 fires the hook as soon as
 * SIGBREAKF_CTRL_C is delivered.  For terminal signals, pthread_exit() is
 * called from within the hook using the longjmp/setjmp mechanism to unwind
 * cleanly back to StarterFunc.
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

static int tests_passed = 0;
static int tests_failed = 0;

static void check(const char *desc, int condition) {
    if (condition) { printf("  PASS: %s\n", desc); tests_passed++; }
    else           { printf("  FAIL: %s\n", desc); tests_failed++; }
}

/* ---- thread helpers ---- */
static void *sleeper_thread(void *arg) { (void)arg; sleep(60); return NULL; }
static void *quick_thread(void *arg)   { (void)arg; usleep(50000); return NULL; }

/* ---- Test 1: sig=0 existence check on live thread ---- */
static void test_sig0_valid_thread(void) {
    puts("\n[Test 1] sig=0 on a live thread");
    pthread_t tid;
    pthread_create(&tid, NULL, sleeper_thread, NULL);
    check("pthread_kill(live_thread, 0) returns 0", pthread_kill(tid, 0) == 0);
    pthread_cancel(tid); pthread_join(tid, NULL);
}

/* ---- Test 2: sig=0 on dead thread returns ESRCH ---- */
static void test_sig0_dead_thread(void) {
    puts("\n[Test 2] sig=0 on dead thread (ESRCH)");
    pthread_t tid;
    pthread_create(&tid, NULL, quick_thread, NULL);
    pthread_join(tid, NULL);
    check("pthread_kill(dead_thread, 0) returns ESRCH", pthread_kill(tid, 0) == ESRCH);
}

/* ---- Test 3: invalid signal number ---- */
static void test_invalid_signal(void) {
    puts("\n[Test 3] invalid signal number (EINVAL)");
    pthread_t tid;
    pthread_create(&tid, NULL, sleeper_thread, NULL);
    check("pthread_kill(thread, -1) returns EINVAL",   pthread_kill(tid, -1)   == EINVAL);
    check("pthread_kill(thread, NSIG) returns EINVAL", pthread_kill(tid, NSIG) == EINVAL);
    check("pthread_kill(thread, 999) returns EINVAL",  pthread_kill(tid, 999)  == EINVAL);
    pthread_cancel(tid); pthread_join(tid, NULL);
}

/* ---- Test 4: SIGTERM to sleeping thread ---- */
static void test_sigterm_sleeping(void) {
    puts("\n[Test 4] SIGTERM to a sleeping thread");
    pthread_t tid;
    pthread_create(&tid, NULL, sleeper_thread, NULL);
    check("pthread_kill(sleeping, SIGTERM) returns 0", pthread_kill(tid, SIGTERM) == 0);
    usleep(200000); pthread_cancel(tid); pthread_join(tid, NULL);
}

/* ---- Test 5: signal to self ---- */
static volatile int self_fired = 0;
static void handler_self(int sig) { (void)sig; self_fired = 1; }

static void test_signal_to_self(void) {
    puts("\n[Test 5] pthread_kill to self with SIGUSR1");
    signal(SIGUSR1, handler_self); self_fired = 0;
    check("pthread_kill(self, SIGUSR1) returns 0", pthread_kill(pthread_self(), SIGUSR1) == 0);
    usleep(50000);
    printf("  INFO: self_fired = %d (1 means handler ran)\n", self_fired);
    signal(SIGUSR1, SIG_DFL);
}

/* ---- Test 6: SIG_IGN ---- */
static void *ignore_thread(void *arg) {
    (void)arg; signal(SIGUSR2, SIG_IGN); sleep(60); return NULL;
}
static void test_sigusr_ignored(void) {
    puts("\n[Test 6] SIGUSR2 with SIG_IGN (returns 0)");
    pthread_t tid;
    pthread_create(&tid, NULL, ignore_thread, NULL);
    usleep(100000);
    check("pthread_kill(thread, SIGUSR2 / SIG_IGN) returns 0", pthread_kill(tid, SIGUSR2) == 0);
    pthread_cancel(tid); pthread_join(tid, NULL);
}

/* ---- Test 7: signal after join returns ESRCH ---- */
static void test_signal_after_join(void) {
    puts("\n[Test 7] pthread_kill after join (ESRCH)");
    pthread_t tid;
    pthread_create(&tid, NULL, quick_thread, NULL);
    pthread_join(tid, NULL);
    check("pthread_kill(joined_thread, SIGUSR1) returns ESRCH", pthread_kill(tid, SIGUSR1) == ESRCH);
}

/* ---- Test 8: SIGTERM to tight-loop thread (exception hook + longjmp) ---- */
static volatile int loop_flag = 0;
static void *tight_loop_thread(void *arg) {
    (void)arg;
    while (1) { loop_flag++; }
    return NULL;
}

static void test_sigterm_tight_loop(void) {
    puts("\n[Test 8] SIGTERM to infinite tight-loop thread");
    puts("  (core test: AmigaOS4 exception hook fires mid-loop, longjmp unwinds)");
    loop_flag = 0;
    pthread_t tid;
    pthread_create(&tid, NULL, tight_loop_thread, NULL);
    usleep(100000);
    check("tight loop thread is running (loop_flag > 0)", loop_flag > 0);

    check("pthread_kill(tight_loop, SIGTERM) returns 0", pthread_kill(tid, SIGTERM) == 0);

    int terminated = 0;
    for (int i = 0; i < 20; i++) {
        usleep(100000);
        if (pthread_kill(tid, 0) == ESRCH) { terminated = 1; break; }
    }
    if (!terminated) pthread_cancel(tid);
    pthread_join(tid, NULL);
    check("tight-loop thread terminated after SIGTERM", terminated);
}

/* ---- Test 9: SIGUSR1 custom handler to tight-loop thread ---- */
static volatile int tight_usr1 = 0;
static void handler_tight(int sig) { (void)sig; tight_usr1 = 1; }

static void *tight_signal_thread(void *arg) {
    (void)arg;
    signal(SIGUSR1, handler_tight);
    while (!tight_usr1) { /* spin until hook fires handler */ }
    return NULL;
}

static void test_sigusr1_tight_loop(void) {
    puts("\n[Test 9] SIGUSR1 custom handler to tight-loop thread");
    puts("  (exception hook calls handler in thread context, thread exits cleanly)");
    tight_usr1 = 0;
    pthread_t tid;
    pthread_create(&tid, NULL, tight_signal_thread, NULL);
    usleep(100000);

    check("pthread_kill(tight_loop, SIGUSR1) returns 0", pthread_kill(tid, SIGUSR1) == 0);

    int joined = 0;
    for (int i = 0; i < 20; i++) {
        usleep(100000);
        if (pthread_kill(tid, 0) == ESRCH) { joined = 1; break; }
    }
    if (!joined) pthread_cancel(tid);
    pthread_join(tid, NULL);
    check("SIGUSR1 handler fired in tight-loop thread", tight_usr1 == 1);
}

/* ---- main ---- */
int main(void) {
    puts("=== pthread_kill test suite ===");
    test_sig0_valid_thread();
    test_sig0_dead_thread();
    test_invalid_signal();
    test_sigterm_sleeping();
    test_signal_to_self();
    test_sigusr_ignored();
    test_signal_after_join();
    test_sigterm_tight_loop();
    test_sigusr1_tight_loop();
    printf("\n=== Results: %d passed, %d failed ===\n", tests_passed, tests_failed);
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
