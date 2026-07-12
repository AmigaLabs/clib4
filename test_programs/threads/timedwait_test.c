#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>

#define IDLE_TIMEOUT_SEC 60
#define NUM_WORKERS      4

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int             shutdown;
    int             use_monotonic;   /* how cond was initialised */
    double          woke_after[NUM_WORKERS];
    int             woke_by_timeout[NUM_WORKERS];
} Pool;

static double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void deadline_realtime(struct timespec *ts, int add_sec)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    ts->tv_sec  = now.tv_sec + add_sec;
    ts->tv_nsec = now.tv_usec * 1000;
}

typedef struct { Pool *p; long id; } WArg;

static void *worker_main(void *arg)
{
    WArg *wa = (WArg *)arg;
    Pool *p = wa->p;
    long id = wa->id;
    double start = now_sec();

    pthread_mutex_lock(&p->mutex);
    /* pool loop: wait until shutdown, timed out at the idle timeout */
    while (!p->shutdown) {
        struct timespec ts;
        deadline_realtime(&ts, IDLE_TIMEOUT_SEC);
        int r = pthread_cond_timedwait(&p->cond, &p->mutex, &ts);
        if (r == ETIMEDOUT) {
            p->woke_by_timeout[id] = 1;
            break;   /* idle timeout elapsed -> pool would drop this thread */
        }
        if (r != 0 && r != ETIMEDOUT) {
            fprintf(stderr, "worker %ld: timedwait rv=%d (%s)\n", id, r, strerror(r));
        }
        /* loop: re-check shutdown predicate */
    }
    p->woke_after[id] = now_sec() - start;
    pthread_mutex_unlock(&p->mutex);
    free(wa);
    return NULL;
}

static int run(int use_monotonic)
{
    Pool p;
    pthread_t th[NUM_WORKERS];

    memset(&p, 0, sizeof(p));
    p.use_monotonic = use_monotonic;
    pthread_mutex_init(&p.mutex, NULL);

    if (use_monotonic) {
        pthread_condattr_t attr;
        pthread_condattr_init(&attr);

        int sr = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        if (sr != 0)
            printf("  (pthread_condattr_setclock(MONOTONIC) rv=%d %s)\n", sr, strerror(sr));
        pthread_cond_init(&p.cond, &attr);
        pthread_condattr_destroy(&attr);
    } else {
        pthread_cond_init(&p.cond, NULL);
    }

    printf("=== Test: cond clock = %s, deadline computed with gettimeofday (REALTIME) ===\n",
           use_monotonic ? "CLOCK_MONOTONIC" : "default(REALTIME)");
    printf("Spawning %d workers, each parked in timedwait(+%ds)...\n",
           NUM_WORKERS, IDLE_TIMEOUT_SEC);
    fflush(stdout);

    for (long i = 0; i < NUM_WORKERS; i++) {
        WArg *wa = (WArg *)malloc(sizeof(WArg));
        wa->p = &p; wa->id = i;
        pthread_create(&th[i], NULL, worker_main, wa);
    }

    /* Give workers time to reach timedwait. */
    usleep(500 * 1000);  /* 0.5s */

    /* Shutdown pattern: set flag + broadcast (should wake ALL workers now). */
    double t_broadcast = now_sec();
    printf("Broadcasting shutdown at t=%.3fs (workers should wake immediately)...\n",
           0.0);
    fflush(stdout);
    pthread_mutex_lock(&p.mutex);
    p.shutdown = 1;
    pthread_cond_broadcast(&p.cond);
    pthread_mutex_unlock(&p.mutex);

    for (int i = 0; i < NUM_WORKERS; i++)
        pthread_join(th[i], NULL);
    double join_elapsed = now_sec() - t_broadcast;

    int bug = 0;
    for (int i = 0; i < NUM_WORKERS; i++) {
        printf("  worker %d: woke %.3fs after start (%s)\n",
               i, p.woke_after[i],
               p.woke_by_timeout[i] ? "BY TIMEOUT - BUG!" : "by broadcast - OK");
        if (p.woke_by_timeout[i])
            bug = 1;
    }
    printf("All workers joined %.3fs after broadcast.\n", join_elapsed);

    if (bug || join_elapsed > 5.0) {
        printf("RESULT: *** BUG *** broadcast did NOT wake the timedwait; "
               "workers waited the full %ds timeout.\n", IDLE_TIMEOUT_SEC);
        printf("        -> This is the powerfox shutdown stall. Fix: poll/cap the "
               "timeout on AmigaOS.\n");
    } else {
        printf("RESULT: OK - broadcast woke all workers promptly.\n");
    }
    printf("\n");
    fflush(stdout);

    pthread_cond_destroy(&p.cond);
    pthread_mutex_destroy(&p.mutex);
    return bug;
}

int main(void)
{
    int bug = 0;

    bug |= run(0);
    /* MONOTONIC-initialised condvar with a REALTIME deadline -- exposes a
     * clock-mismatch bug if present. */
    bug |= run(1);

    printf("=========================================================\n");
    printf("Overall: %s\n", bug ? "BUG REPRODUCED" : "no stall reproduced");
    return bug ? 1 : 0;
}
