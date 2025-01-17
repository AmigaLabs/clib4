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

int main (int argc, char *argv[])
{
    sigset_t mask;
    sigset_t orig_mask;
    struct timespec timeout;

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