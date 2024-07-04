#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

void timer_handler(int signum) {
    static int count = 0;
    printf("timer expired %d times\n", ++count);
}

void timer_setting(int elapstime) {

    struct sigaction sa;
    struct itimerval timer;

    /* Install timer_handler as the signal handler for SIGVTALRM. */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &timer_handler;
    sigaction(SIGALRM, &sa, NULL);

    /* Configure the timer to expire after 250 msec... */

    timer.it_value.tv_sec = 1; //<-- set to start after 1 sec
    timer.it_value.tv_usec = 0;

    /* ... and every 250 msec after that. */

    timer.it_interval.tv_sec = elapstime; //<--- TIME (seconds)  gets set here from prams
    timer.it_interval.tv_usec = 0;

    /* Start a virtual timer. It counts down whenever this process is
      executing. */

    setitimer(ITIMER_REAL, &timer, NULL);

    /* Do busy work. */
    while (1) {
        usleep(10);
    };
}

int main() {
    timer_setting(3);
    return 0;
}
