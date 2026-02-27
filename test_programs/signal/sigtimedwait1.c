#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define INTERVAL 500        /* number of milliseconds to go off */

void alarm_handler(int sig) {
    printf("Timer went off. getitimer.\n");
}

int main (int argc, char *argv[])
{
    struct itimerval it_val;  /* for setting itimer */
    sigset_t mask;
    sigset_t orig_mask;
    struct timespec timeout;

 	if (signal(SIGALRM, (void (*)(int)) alarm_handler) == SIG_ERR) {
        perror("Unable to catch SIGALRM");
        exit(1);
    }

	it_val.it_value.tv_sec = INTERVAL / 1000;
    it_val.it_value.tv_usec = (INTERVAL * 1000) % 1000000;
	it_val.it_interval.tv_sec = 0;
	it_val.it_interval.tv_usec = 0;

	printf("Setting itimer with %ld s and %d us,\n",it_val.it_value.tv_sec, it_val.it_value.tv_usec);

    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        perror("error calling setitimer()");
        exit(1);
    }

    sigemptyset (&mask);
    sigaddset (&mask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
        perror ("sigprocmask");
        return 1;
    }

    timeout.tv_sec = 10;
    timeout.tv_nsec = 0;

 	printf ("Waiting for %lld s and %ld ns or CTRL-C\n",timeout.tv_sec,timeout.tv_nsec);

    int v =sigtimedwait(&mask, NULL, &timeout);
    if (errno == EAGAIN) {
        printf ("Timeout\n");
    }
	else if(v== SIGINT){
        printf("SIGINT\n");
     }

    return 0;
}