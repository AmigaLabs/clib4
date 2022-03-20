/*
 * setitimer.c - simple use of the interval timer
 */

#include <sys/time.h>       /* for setitimer */
#include <unistd.h>     /* for pause */
#include <signal.h>     /* for signal */
#include <stdio.h>
#include <stdlib.h>

#define INTERVAL 500        /* number of milliseconds to go off */

/* function prototype */
void DoStuff(void);

enum {
    SECS_TO_SLEEP = 10, NSEC_TO_SLEEP = 125
};

int main(int argc, char *argv[]) {

    struct itimerval it_val;  /* for setting itimer */
    struct timespec remaining, request = {SECS_TO_SLEEP, NSEC_TO_SLEEP};

    /* Upon SIGALRM, call DoStuff().
     * Set interval timer.  We want frequency in ms,
     * but the setitimer call needs seconds and useconds. */
    if (signal(SIGALRM, (void (*)(int)) DoStuff) == SIG_ERR) {
        perror("Unable to catch SIGALRM");
        exit(1);
    }
    it_val.it_value.tv_sec = INTERVAL / 1000;
    it_val.it_value.tv_usec = (INTERVAL * 1000) % 1000000;
    it_val.it_interval = it_val.it_value;
    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        perror("error calling setitimer()");
        exit(1);
    }
    /* nanosleep will be interrupted by SIGALRM */
    nanosleep(&request, &remaining);
}

/*
 * DoStuff
 */
void DoStuff(void) {
    struct itimerval old_value;
    getitimer(ITIMER_REAL, &old_value);
    printf("Timer went off. getitimer now is %d sec, %d msec.\n",
           (int) old_value.it_value.tv_sec,
           (int) old_value.it_value.tv_usec);
}