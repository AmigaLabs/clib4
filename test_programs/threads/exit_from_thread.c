/*
 * exit(), _exit() and abort() called on a pthread must end the whole
 * process (POSIX), whichever thread the main thread is waiting for.
 *
 * Each case runs a copy of this program as a child. In the child, the main
 * thread waits in pthread_join() for a thread parked in pthread_cond_wait()
 * that never ends by itself, while a second thread calls the function. The
 * parent checks the child's return code and what it printed:
 *
 *   abort    the child ends with a failure code; no atexit() function runs
 *   exit     the child ends with 20; its atexit() function runs once
 *   _exit    the child ends with 7; no atexit() function runs
 *
 * Before the fix the child never ended: _exit() on a thread removed only
 * that task, and the main thread waited in pthread_join() for ever.
 *
 * Run as:  exit_from_thread
 *
 * The program runs copies of itself, so start it with a path that can be
 * run again (for example from its own drawer).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define OUT "T:exit_from_thread.out"
#define ERR "T:exit_from_thread.err"

static const char *self;
static int failures;
static const char *mode;

static void check(const char *what, int ok) {
    printf("  %s: %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok)
        failures++;
}

static void at_exit_fn(void) {
    printf("atexit ran\n");
}

static pthread_mutex_t park_m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t park_c = PTHREAD_COND_INITIALIZER;

static void *parked(void *arg) {
    (void) arg;
    pthread_mutex_lock(&park_m);
    for (;;)
        pthread_cond_wait(&park_c, &park_m);
    return NULL;
}

static void *ender(void *arg) {
    (void) arg;
    usleep(200000);
    if (!strcmp(mode, "abort"))
        abort();
    if (!strcmp(mode, "exit"))
        exit(20);
    _exit(7);
    return NULL;
}

static int child(void) {
    pthread_t p, e;

    atexit(at_exit_fn);
    pthread_create(&p, NULL, parked, NULL);
    pthread_create(&e, NULL, ender, NULL);
    pthread_join(p, NULL);
    printf("main returned from pthread_join\n");
    return 0;
}

/* Runs "self mode" with its output in OUT; the return code, and whether
 * the output holds `line`. */
static int run(const char *m, const char *line, int *found) {
    char cmd[512];
    char buf[256];
    FILE *f;
    int rc;

    snprintf(cmd, sizeof(cmd), "%s %s <NIL: >" OUT " *>" ERR, self, m);
    rc = system(cmd);
    *found = 0;
    if ((f = fopen(OUT, "r")) != NULL) {
        while (fgets(buf, sizeof(buf), f) != NULL)
            if (strstr(buf, line) != NULL)
                (*found)++;
        fclose(f);
    }
    unlink(OUT);
    unlink(ERR);
    return rc;
}

int main(int argc, char **argv) {
    int rc, found;

    if (argc > 1) {
        mode = argv[1];
        return child();
    }
    self = argv[0];

    printf("abort() on a thread:\n");
    rc = run("abort", "atexit ran", &found);
    check("the process ends with a failure code", rc != 0);
    check("no atexit() function runs", found == 0);

    printf("exit(20) on a thread:\n");
    rc = run("exit", "atexit ran", &found);
    check("the process ends with 20", rc == 20);
    check("the atexit() function runs once", found == 1);

    printf("_exit(7) on a thread:\n");
    rc = run("_exit", "atexit ran", &found);
    check("the process ends with 7", rc == 7);
    check("no atexit() function runs", found == 0);

    printf("%s\n", failures ? "FAILED" : "all passed");
    return failures ? 1 : 0;
}
