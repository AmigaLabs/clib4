/*
 * setitimer.c - simple use of the interval timer
 *
 * Expected output, similar to this:
 *
 * >Setting itimer with 0 s and 500000 ms, repeating
 * >Going to sleep for 10 s and 125 ms
 * >Timer went off. getitimer now is 0 sec, 495028 msec.
 * >Signal distribe me, Remaining time 9 s and 494991904 ms!
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

	printf("Setting itimer with %ld s and %d us, repeating\n",it_val.it_value.tv_sec, it_val.it_value.tv_usec);

    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        perror("error calling setitimer()");
        exit(1);
    }

	printf("Going to sleep for 10 s and 125 ns\n");
    /* nanosleep will be interrupted by SIGALRM */
    int result = nanosleep(&request, &remaining);
	if( result == 0 ) {
		printf("Slept for the desired time!\n");
	}
	else if( result == -1 ) {
		if( errno == EINVAL ) {
			printf("Passed in values no valid\n");
		}
		else if( errno == EINTR ) {
			printf("Signal distrubed me, Remaining time %ld s and %ld us!\n",remaining.tv_sec, remaining.tv_nsec);
		}
		else {
			printf("Unknown err no: %d!\n",errno);
		}
	}
	else {
		printf("Don't know why i Got woke up!\n");
	}
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