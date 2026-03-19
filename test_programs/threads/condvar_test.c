/*
 * clib4_condvar_test.c
 *
 * Riproduce esattamente il pattern usato da PowerFox durante lo shutdown:
 *   - Main thread: pthread_cond_wait senza timeout (come nsEventQueue)
 *   - Timer thread: pthread_cond_timedwait + signal (come TimerThread)
 *   - PR_Sleep simulation: timedwait su cond dummy
 *
 * Compila: gcc -o clib4_condvar_test clib4_condvar_test.c -lpthread
 * Esegui : ./clib4_condvar_test
 *
 * Tutti i test devono completarsi. Se un test si blocca, quella e' la root cause.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#define COL_GREEN  "\033[32m"
#define COL_RED    "\033[31m"
#define COL_RESET  "\033[0m"

static int g_failures = 0;

/* ------------------------------------------------------------------ */
/* Utility: elapsed ms between two timespec                           */
/* ------------------------------------------------------------------ */
static long elapsed_ms(const struct timespec *a, const struct timespec *b)
{
    return (b->tv_sec - a->tv_sec) * 1000L
         + (b->tv_nsec - a->tv_nsec) / 1000000L;
}

/* ------------------------------------------------------------------ */
/* Utility: add N milliseconds to a CLOCK_REALTIME timespec           */
/* ------------------------------------------------------------------ */
static void ts_add_ms(struct timespec *ts, int ms)
{
    ts->tv_sec  += ms / 1000;
    ts->tv_nsec += (long)(ms % 1000) * 1000000L;
    while (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

static void pass(const char *name)
{
    printf("[%sPASS%s] %s\n", COL_GREEN, COL_RESET, name);
}

static void fail(const char *name, const char *reason)
{
    printf("[%sFAIL%s] %s: %s\n", COL_RED, COL_RESET, name, reason);
    g_failures++;
}

/* ==================================================================
 * TEST 1: pthread_cond_timedwait deve tornare ETIMEDOUT
 *
 * Simula: PR_Sleep(25ms) / PR_Sleep(500ms) / WaitForAll timeout
 * ================================================================== */
static void test_timedwait_timeout(const char *name, int timeout_ms)
{
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  c = PTHREAD_COND_INITIALIZER;
    struct timespec start, end, deadline;
    //printf("Calling pthread_cond_timedwait with %d ms timeout...\n", timeout_ms);
    clock_gettime(CLOCK_REALTIME, &start);
    printf("  start at %ld.%09ld\n", start.tv_sec, start.tv_nsec);
    deadline = start;
    ts_add_ms(&deadline, timeout_ms);
    printf("  deadline at %ld.%09ld\n", deadline.tv_sec, deadline.tv_nsec);
    pthread_mutex_lock(&m);
    printf("  waiting...\n");
    int rv = pthread_cond_timedwait(&c, &m, &deadline);
    printf("  pthread_cond_timedwait returned %d (%s)\n", rv, strerror(rv));
    pthread_mutex_unlock(&m);
    printf("  done waiting, measuring elapsed time...\n");
    clock_gettime(CLOCK_REALTIME, &end);
    long spent = elapsed_ms(&start, &end);

    if (rv != ETIMEDOUT) {
        char buf[128];
        snprintf(buf, sizeof(buf), "expected ETIMEDOUT, got %d (%s)", rv, strerror(rv));
        fail(name, buf);
    } else if (spent < timeout_ms - 50 || spent > timeout_ms + 1000) {
        char buf[128];
        snprintf(buf, sizeof(buf), "took %ld ms (expected ~%d ms)", spent, timeout_ms);
        fail(name, buf);
    } else {
        printf("  elapsed %ld ms (expected ~%d ms)\n", spent, timeout_ms);
        pass(name);
    }
    //printf("Destroying mutex and condvar...\n");
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&c);
    //printf("Done.\n");
}

/* ==================================================================
 * TEST 2: pthread_cond_wait svegliato da pthread_cond_signal
 *
 * Simula: nsEventQueue::GetEvent(true) svegliato da PutEvent()
 *         (equivalente a AsyncShutdown spinner svegliato dal bailout timer)
 * ================================================================== */
typedef struct { pthread_mutex_t m; pthread_cond_t c; volatile int v; int delay_ms; } t2_state;

static void* t2_signaler(void *arg)
{
    t2_state *s = (t2_state *)arg;
    struct timespec ts = { 0, (long)s->delay_ms * 1000000L };
    while (ts.tv_nsec >= 1000000000L) { ts.tv_sec++; ts.tv_nsec -= 1000000000L; }
    nanosleep(&ts, NULL);

    pthread_mutex_lock(&s->m);
    s->v = 1;
    pthread_cond_signal(&s->c);
    pthread_mutex_unlock(&s->m);
    return NULL;
}

static void test_signal_wakes_wait(const char *name, int delay_ms)
{
    t2_state s = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, delay_ms };
    pthread_t t;
    struct timespec start, end;

    pthread_create(&t, NULL, t2_signaler, &s);
    clock_gettime(CLOCK_REALTIME, &start);

    pthread_mutex_lock(&s.m);
    while (!s.v) {
        /* Exactly like nsEventQueue: wait with NO timeout */
        pthread_cond_wait(&s.c, &s.m);
    }
    pthread_mutex_unlock(&s.m);

    clock_gettime(CLOCK_REALTIME, &end);
    long spent = elapsed_ms(&start, &end);

    pthread_join(t, NULL);

    if (!s.v) {
        fail(name, "state variable never set");
    } else if (spent < delay_ms - 50 || spent > delay_ms + 2000) {
        char buf[128];
        snprintf(buf, sizeof(buf), "took %ld ms (signal delay was %d ms)", spent, delay_ms);
        fail(name, buf);
    } else {
        printf("  woke up after %ld ms (signal delay %d ms)\n", spent, delay_ms);
        pass(name);
    }

    pthread_mutex_destroy(&s.m);
    pthread_cond_destroy(&s.c);
}

/* ==================================================================
 * TEST 3: Simula exactamente TimerThread + main event queue
 *
 * Il timer thread:
 *   - dorme indefinitamente finche' non arriva un timer (pthread_cond_wait)
 *   - quando ha un timer, dorme con timedwait per N ms
 *   - allo scadere, posta un evento nella coda del main thread
 *
 * Il main thread:
 *   - aspetta sulla coda principale senza timeout (pthread_cond_wait)
 *   - si sveglia quando il timer thread posta l'evento
 *
 * Questo e' esattamente cio' che non funziona durante lo shutdown di PowerFox.
 * ================================================================== */
typedef struct {
    /* main event queue */
    pthread_mutex_t main_lock;
    pthread_cond_t  main_avail;
    volatile int    main_count;

    /* timer thread control */
    pthread_mutex_t timer_lock;
    pthread_cond_t  timer_cond;
    volatile int    timer_next_ms;   /* 0 = nessun timer schedulato */
    volatile int    timer_shutdown;
} t3_state;

static void t3_queue_post(t3_state *s)
{
    pthread_mutex_lock(&s->main_lock);
    s->main_count++;
    pthread_cond_signal(&s->main_avail);
    pthread_mutex_unlock(&s->main_lock);
}

static void t3_queue_wait(t3_state *s)
{
    pthread_mutex_lock(&s->main_lock);
    while (s->main_count == 0) {
        pthread_cond_wait(&s->main_avail, &s->main_lock);
    }
    s->main_count--;
    pthread_mutex_unlock(&s->main_lock);
}

static void t3_schedule(t3_state *s, int delay_ms)
{
    pthread_mutex_lock(&s->timer_lock);
    s->timer_next_ms = delay_ms;
    pthread_cond_signal(&s->timer_cond);
    pthread_mutex_unlock(&s->timer_lock);
}

static void* t3_timer_thread(void *arg)
{
    t3_state *s = (t3_state *)arg;

    while (1) {
        pthread_mutex_lock(&s->timer_lock);

        /* Aspetta un timer da schedulare (come TimerThread con nessun timer) */
        while (s->timer_next_ms == 0 && !s->timer_shutdown) {
            pthread_cond_wait(&s->timer_cond, &s->timer_lock);
        }
        if (s->timer_shutdown) {
            pthread_mutex_unlock(&s->timer_lock);
            break;
        }
        int delay = s->timer_next_ms;
        s->timer_next_ms = 0;
        pthread_mutex_unlock(&s->timer_lock);

        /* Dorme fino alla scadenza del timer con timedwait */
        struct timespec deadline;
        clock_gettime(CLOCK_REALTIME, &deadline);
        ts_add_ms(&deadline, delay);

        pthread_mutex_lock(&s->timer_lock);
        int rv = pthread_cond_timedwait(&s->timer_cond, &s->timer_lock, &deadline);
        pthread_mutex_unlock(&s->timer_lock);

        if (rv == ETIMEDOUT) {
            /* Il timer e' scaduto: posta evento al main thread */
            t3_queue_post(s);
        }
        /* se rv == 0: il timer e' stato cancellato/sostituito */
    }
    return NULL;
}

static void test_timer_to_main_queue(const char *name, int timer_ms, int iterations)
{
    t3_state s;
    memset(&s, 0, sizeof(s));
    pthread_mutex_init(&s.main_lock, NULL);
    pthread_cond_init(&s.main_avail, NULL);
    pthread_mutex_init(&s.timer_lock, NULL);
    pthread_cond_init(&s.timer_cond, NULL);

    pthread_t t;
    pthread_create(&t, NULL, t3_timer_thread, &s);

    int fail_count = 0;
    struct timespec start, end;

    for (int i = 0; i < iterations; i++) {
        clock_gettime(CLOCK_REALTIME, &start);

        t3_schedule(&s, timer_ms);

        /*
         * Aspetta sulla coda SENZA timeout — esattamente come
         * AsyncShutdown.jsm Spinner con processNextEvent(true).
         * Se il timer thread non sveglia il main thread, questo BLOCCA.
         * Usiamo una safety net con timedwait a 5s per non bloccare il test.
         */
        pthread_mutex_lock(&s.main_lock);
        int got = 0;
        while (s.main_count == 0) {
            struct timespec safety = start;
            ts_add_ms(&safety, timer_ms + 5000);
            int rv = pthread_cond_timedwait(&s.main_avail, &s.main_lock, &safety);
            if (rv == ETIMEDOUT) break;
        }
        if (s.main_count > 0) { s.main_count--; got = 1; }
        pthread_mutex_unlock(&s.main_lock);

        clock_gettime(CLOCK_REALTIME, &end);
        long spent = elapsed_ms(&start, &end);

        if (!got) {
            printf("  iter %d: FAILED (timer did not fire after %ld ms)\n", i, spent);
            fail_count++;
        }
    }

    /* Shutdown timer thread */
    pthread_mutex_lock(&s.timer_lock);
    s.timer_shutdown = 1;
    pthread_cond_signal(&s.timer_cond);
    pthread_mutex_unlock(&s.timer_lock);
    pthread_join(t, NULL);

    if (fail_count == 0) {
        printf("  %d/%d iterations OK\n", iterations, iterations);
        pass(name);
    } else {
        char buf[64];
        snprintf(buf, sizeof(buf), "%d/%d iterations FAILED", fail_count, iterations);
        fail(name, buf);
    }

    pthread_mutex_destroy(&s.main_lock);
    pthread_cond_destroy(&s.main_avail);
    pthread_mutex_destroy(&s.timer_lock);
    pthread_cond_destroy(&s.timer_cond);
}

/* ==================================================================
 * TEST 4: pthread_cond_signal inviato PRIMA che il waiter entri in wait
 *         (race classica — deve funzionare grazie al mutex)
 * ================================================================== */
typedef struct { pthread_mutex_t m; pthread_cond_t c; volatile int v; } t4_state;

static void* t4_early_signaler(void *arg)
{
    t4_state *s = (t4_state *)arg;
    /* Segnala immediatamente, senza aspettare il waiter */
    pthread_mutex_lock(&s->m);
    s->v = 1;
    pthread_cond_signal(&s->c);
    pthread_mutex_unlock(&s->m);
    return NULL;
}

static void test_signal_before_wait(const char *name)
{
    t4_state s = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0 };
    pthread_t t;

    /* Il signaler gira prima, posta il segnale prima che il main entri in wait */
    pthread_create(&t, NULL, t4_early_signaler, &s);
    pthread_join(t, NULL);  /* Assicurati che abbia segnalato */

    /* Il main controlla lo stato prima di aspettare — questo e' il pattern
     * corretto con mutex (il segnale non va perso se il lock e' usato bene) */
    pthread_mutex_lock(&s.m);
    /* s.v deve essere gia' 1, quindi non entriamo nel wait */
    int waited = 0;
    if (!s.v) {
        waited = 1;
        /* Questo non dovrebbe succedere se il signaler ha acquistato il mutex correttamente */
        struct timespec deadline;
        clock_gettime(CLOCK_REALTIME, &deadline);
        ts_add_ms(&deadline, 2000);
        pthread_cond_timedwait(&s.c, &s.m, &deadline);
    }
    int ok = (s.v == 1);
    pthread_mutex_unlock(&s.m);

    if (ok) {
        printf("  state was set before wait (waited=%d)\n", waited);
        pass(name);
    } else {
        fail(name, "state never set");
    }

    pthread_mutex_destroy(&s.m);
    pthread_cond_destroy(&s.c);
}

/* ==================================================================
 * TEST 5: PR_Sleep simulation (timedwait su dummy mutex/cond)
 *         Esattamente come NSPR implementa PR_Sleep su clib4.
 * ================================================================== */
static void test_pr_sleep_simulation(const char *name, int sleep_ms)
{
    /* NSPR alloca un mutex e condvar temporanei per PR_Sleep */
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  c = PTHREAD_COND_INITIALIZER;

    struct timespec start, deadline, end;
    clock_gettime(CLOCK_REALTIME, &start);
    deadline = start;
    ts_add_ms(&deadline, sleep_ms);

    pthread_mutex_lock(&m);
    int rv = pthread_cond_timedwait(&c, &m, &deadline);
    pthread_mutex_unlock(&m);

    clock_gettime(CLOCK_REALTIME, &end);
    long spent = elapsed_ms(&start, &end);

    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&c);

    if (rv != ETIMEDOUT) {
        char buf[128];
        snprintf(buf, sizeof(buf), "expected ETIMEDOUT, got %d", rv);
        fail(name, buf);
    } else if (spent < sleep_ms - 50 || spent > sleep_ms + 2000) {
        char buf[128];
        snprintf(buf, sizeof(buf), "took %ld ms (expected ~%d ms)", spent, sleep_ms);
        fail(name, buf);
    } else {
        printf("  slept %ld ms (requested %d ms)\n", spent, sleep_ms);
        pass(name);
    }
}

/* ==================================================================
 * TEST 6: pthread_join dopo exit del thread
 * ================================================================== */
static void* t6_quick_exit(void *arg) { (void)arg; return NULL; }
static void* t6_delayed_exit(void *arg)
{
    int *ms = (int*)arg;
    struct timespec ts = { 0, (long)(*ms) * 1000000L };
    while (ts.tv_nsec >= 1000000000L) { ts.tv_sec++; ts.tv_nsec -= 1000000000L; }
    nanosleep(&ts, NULL);
    return NULL;
}

static void test_pthread_join(const char *name, int delay_ms)
{
    pthread_t t;
    struct timespec start, end;

    clock_gettime(CLOCK_REALTIME, &start);
    if (delay_ms == 0) {
        pthread_create(&t, NULL, t6_quick_exit, NULL);
    } else {
        pthread_create(&t, NULL, t6_delayed_exit, &delay_ms);
    }
    pthread_join(t, NULL);
    clock_gettime(CLOCK_REALTIME, &end);

    long spent = elapsed_ms(&start, &end);
    if (spent > delay_ms + 2000) {
        char buf[128];
        snprintf(buf, sizeof(buf), "pthread_join took %ld ms (expected ~%d ms)", spent, delay_ms);
        fail(name, buf);
    } else {
        printf("  join returned after %ld ms\n", spent);
        pass(name);
    }
}

/* ==================================================================
 * MAIN
 * ================================================================== */
int main(void)
{
    printf("=== PowerFox / clib4 condvar & pthread shutdown test ===\n\n");

    /* --- Test 1: timedwait timeout --- */
    printf("[Test 1] pthread_cond_timedwait deve tornare ETIMEDOUT\n");
    test_timedwait_timeout("timedwait_25ms",  25);
    test_timedwait_timeout("timedwait_500ms", 500);
    test_timedwait_timeout("timedwait_2s",    2000);
    printf("\n");

    /* --- Test 2: cond_signal sveglia cond_wait --- */
    printf("[Test 2] pthread_cond_signal sveglia pthread_cond_wait (no timeout)\n");
    test_signal_wakes_wait("signal_wakes_wait_100ms",  100);
    test_signal_wakes_wait("signal_wakes_wait_500ms",  500);
    test_signal_wakes_wait("signal_wakes_wait_1000ms", 1000);
    printf("\n");

    /* --- Test 3: TimerThread + main event queue (scenario reale) --- */
    printf("[Test 3] Simulazione TimerThread -> main event queue\n");
    printf("         (esatto pattern di PowerFox shutdown)\n");
    test_timer_to_main_queue("timer_queue_500ms_x5",  500, 5);
    test_timer_to_main_queue("timer_queue_100ms_x20", 100, 20);
    test_timer_to_main_queue("timer_queue_50ms_x50",   50, 50);
    printf("\n");

    /* --- Test 4: signal prima del wait --- */
    printf("[Test 4] Signal inviato prima del wait (race con mutex)\n");
    test_signal_before_wait("signal_before_wait");
    printf("\n");

    /* --- Test 5: PR_Sleep simulation --- */
    printf("[Test 5] PR_Sleep simulation (timedwait su dummy cond)\n");
    test_pr_sleep_simulation("pr_sleep_25ms",  25);
    test_pr_sleep_simulation("pr_sleep_100ms", 100);
    test_pr_sleep_simulation("pr_sleep_500ms", 500);
    printf("\n");

    /* --- Test 6: pthread_join --- */
    printf("[Test 6] pthread_join timing\n");
    test_pthread_join("join_immediate",  0);
    test_pthread_join("join_after_500ms", 500);
    printf("\n");

    /* --- Risultato --- */
    printf("=== Risultato: %d failure(s) ===\n", g_failures);
    if (g_failures == 0) {
        printf("%sAll tests PASSED%s\n", COL_GREEN, COL_RESET);
    } else {
        printf("%s%d test(s) FAILED%s\n", COL_RED, g_failures, COL_RESET);
        printf("\nSuggerimento per debug clib4:\n");
        printf("  - Se Test 1/5 fallisce: pthread_cond_timedwait non tornano al timeout\n");
        printf("    -> controlla _pt_TimedWait in nspr/pr/src/pthreads/ptsynch.c\n");
        printf("    -> controlla clock_gettime(CLOCK_REALTIME) in clib4\n");
        printf("  - Se Test 2/3 fallisce: pthread_cond_signal non sveglia il waiter\n");
        printf("    -> controlla pthread_cond_signal in clib4\n");
        printf("    -> potrebbe essere un problema di spurious wakeup handling\n");
        printf("  - Se Test 3 fallisce intermittentemente: race condition nel condvar\n");
    }
    return g_failures ? 1 : 0;
}
