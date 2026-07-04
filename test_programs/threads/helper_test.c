#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

/* ---- configuration ---- */
#define NUM_HELPERS      6      
#define CHURN_ITERATIONS 20000  /* how many wake/sleep cycles before shutdown */
#define SHUTDOWN_ROUNDS  200    /* how many full startup+shutdown cycles      */

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  producer;   /* "PRODUCER" wakeup used by threadLoop        */
    pthread_cond_t  consumer;   /* "CONSUMER" wakeup (main waits on this)       */
    int             has_work;   /* set by main, consumed by a helper           */
    int             terminate;  /* set by main at shutdown                      */
    int             work_done;  /* helper signals back                          */
    int             use_poll;   /* wait mode                                    */
} HelperState;

static HelperState g;

static void cond_wait_poll(pthread_cond_t *cv, pthread_mutex_t *mx)
{
    struct timespec now, ts;
    clock_gettime(CLOCK_REALTIME, &now);
    ts.tv_sec  = now.tv_sec;
    ts.tv_nsec = now.tv_nsec + 100L * 1000L * 1000L; /* +100 ms */
    if (ts.tv_nsec >= 1000000000L) { ts.tv_sec++; ts.tv_nsec -= 1000000000L; }
    int r = pthread_cond_timedwait(cv, mx, &ts);
    if (r != 0 && r != ETIMEDOUT) {
        fprintf(stderr, "pthread_cond_timedwait unexpected rv=%d (%s)\n",
                r, strerror(r));
        abort();
    }
}

/* One helper thread: models HelperThread::threadLoop(). */
static void *helper_main(void *arg)
{
    long id = (long)arg;
    char name[32];
    snprintf(name, sizeof(name), "JS Helper %ld", id);
    pthread_setname_np(pthread_self(), name);

    pthread_mutex_lock(&g.lock);
    for (;;) {
        /* Block until there is work or we must terminate -- exactly the inner
         * "while (true) { if (terminate) return; if (canStart...) break;
         *                 wait(PRODUCER); }" loop in threadLoop(). */
        while (!g.has_work && !g.terminate) {
            if (g.use_poll)
                cond_wait_poll(&g.producer, &g.lock);
            else
                pthread_cond_wait(&g.producer, &g.lock);
        }
        if (g.terminate) {
            pthread_mutex_unlock(&g.lock);
            return NULL;
        }
        /* "do the work" */
        g.has_work = 0;
        g.work_done = 1;
        pthread_cond_broadcast(&g.consumer);  /* wake main (CONSUMER) */
    }
}

/* Post one unit of work and wait for a helper to pick it up */
static void post_work_and_wait(void)
{
    pthread_mutex_lock(&g.lock);
    g.has_work = 1;
    g.work_done = 0;
    pthread_cond_broadcast(&g.producer);
    while (!g.work_done) {
        if (g.use_poll)
            cond_wait_poll(&g.consumer, &g.lock);
        else
            pthread_cond_wait(&g.consumer, &g.lock);
    }
    pthread_mutex_unlock(&g.lock);
}

static int run_one_round(int round, int use_poll)
{
    pthread_t th[NUM_HELPERS];

    /* init shared state fresh each round */
    memset(&g, 0, sizeof(g));
    pthread_mutex_init(&g.lock, NULL);
    pthread_cond_init(&g.producer, NULL);
    pthread_cond_init(&g.consumer, NULL);
    g.use_poll = use_poll;

    /* spawn joinable helpers */
    for (long i = 0; i < NUM_HELPERS; i++) {
        if (pthread_create(&th[i], NULL, helper_main, (void *)i) != 0) {
            fprintf(stderr, "pthread_create failed\n");
            return 1;
        }
    }

    /* churn the CV hard: this is where a lost wake-up race would show up.
     * Alternate between the ordered post/collect (models orderly task
     * handoff) and a "storm" of bare broadcasts with no matching work, which
     * maximises the window in which a helper is between the predicate re-check
     * and the cond_wait call -- the classic lost-wakeup race. */
    for (int i = 0; i < CHURN_ITERATIONS; i++) {
        post_work_and_wait();
        if ((i & 7) == 0) {
            /* Bare broadcast storm: wake helpers with nothing to do so they
             * immediately go back to wait().  If a broadcast lands in the
             * gap, clib4 must still not drop it. */
            pthread_mutex_lock(&g.lock);
            pthread_cond_broadcast(&g.producer);
            pthread_mutex_unlock(&g.lock);
        }
    }

    /* 
     * for each helper { lock; terminate=true; broadcast(PRODUCER); unlock;
     *                   join } */
    for (long i = 0; i < NUM_HELPERS; i++) {
        pthread_mutex_lock(&g.lock);
        g.terminate = 1;
        pthread_cond_broadcast(&g.producer);
        pthread_mutex_unlock(&g.lock);

        /* This is HelperThread::destroy()'s thread->join(). If clib4 lost the
         * broadcast and the helper is still parked in cond_wait (infinite
         * mode), this join() never returns -> the powerfox shutdown hang.
         * The "joining"/"joined" pair lets you spot the hung helper on the
         * target: if you see "joining helper N" with no "joined helper N",
         * that helper is stuck in pthread_cond_wait with a lost broadcast. */
        printf("  round %d: joining helper %ld...\n", round, i);
        fflush(stdout);
        void *ret = NULL;
        int jr = pthread_join(th[i], &ret);
        if (jr != 0) {
            fprintf(stderr, "[round %d] pthread_join(%ld) FAILED rv=%d (%s)\n",
                    round, i, jr, strerror(jr));
            return 1;
        }
        printf("  round %d: joined helper %ld\n", round, i);
        fflush(stdout);
    }

    pthread_cond_destroy(&g.producer);
    pthread_cond_destroy(&g.consumer);
    pthread_mutex_destroy(&g.lock);
    return 0;
}

int main(int argc, char **argv)
{
    int use_poll = 0;
    if (argc > 1 && strcmp(argv[1], "poll") == 0)
        use_poll = 1;
    else if (argc > 1 && strcmp(argv[1], "infinite") == 0)
        use_poll = 0;
    else {
        printf("usage: %s [infinite|poll]\n", argv[0]);
        printf("  infinite = raw pthread_cond_wait \n");
        printf("  poll     = pthread_cond_timedwait 100ms (clib4 fix)\n");
        return 2;
    }

    printf("Mode: %s, %d helpers, %d churn iters, %d rounds\n",
           use_poll ? "POLL (fix)" : "INFINITE (upstream)",
           NUM_HELPERS, CHURN_ITERATIONS, SHUTDOWN_ROUNDS);
    fflush(stdout);

    for (int round = 0; round < SHUTDOWN_ROUNDS; round++) {
        printf("round %d: starting %d helpers, churning, then shutdown...\n",
               round, NUM_HELPERS);
        fflush(stdout);

        if (run_one_round(round, use_poll) != 0) {
            printf("round %d: FAILED\n", round);
            return 1;
        }
        printf("round %d: all %d helpers joined cleanly\n", round, NUM_HELPERS);
        fflush(stdout);
    }

    printf("SUCCESS: %d rounds completed, no hung join, no lost wakeup\n",
           SHUTDOWN_ROUNDS);
    return 0;
}
