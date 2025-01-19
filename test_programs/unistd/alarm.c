#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_LOOP  800000L    /* loop cycle (system dependentant) */

static void wait_alarm(int isignal) {
    puts("");
    puts("*****************************************************************");
    printf("\n\t\tGot signal %d - End program\n", isignal);
    puts("*****************************************************************");

	/*
	From the README, so this example fails
	Don't call exit() function in an alarm() handler otherwise your program will be stuck at exit.
	*/
    exit(0);
}
static void traffic(void) {
    long j;

    while (1) {    /* infinite loop */
        printf("\r-");
        fflush(stdout);
        for (j = MAX_LOOP; j > 0; j--)    /* consume time */
            ;
        printf("\r\\");
        fflush(stdout);
        for (j = MAX_LOOP; j > 0; j--)    /* consume time */
            ;
        printf("\r|");
        fflush(stdout);
        for (j = MAX_LOOP; j > 0; j--)    /* consume time */
            ;
        printf("\r/");
        fflush(stdout);
        for (j = MAX_LOOP; j > 0; j--)    /* consume time */
            ;
    }
}

int main(int argc, char *argv[]) {
    unsigned int sec;

    signal(SIGALRM, wait_alarm); /* SIGALRM handler */

    /* Set 5 seconds alarm */
    sec = alarm(5);
    puts("*****************************************************************");
    printf(" previous ALARM set on %u seconds\n", sec);
    puts("*****************************************************************");

    /* Set 10 seconds alarm */
    sec = alarm(10);
    puts("*****************************************************************");
    printf(" previous ALARM set on %u seconds\n", sec);
    puts("*****************************************************************");

    traffic();    /* show some stuff */

    return 0;
}

