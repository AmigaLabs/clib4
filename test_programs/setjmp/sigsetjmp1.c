#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

jmp_buf env;

static void signal_handler(int sig);
int processing = 1;

int main() {
    int returned_from_longjump;
    unsigned int time_interval = 4;
    if ((returned_from_longjump = setjmp(env)) != 0) {
        switch (returned_from_longjump) {
            case SIGINT:
                printf("longjumped from interrupt %d\n", SIGINT);
                break;
            case SIGALRM:
                printf("longjumped from alarm %d\n", SIGALRM);
                break;
        }
    }
    signal(SIGINT, signal_handler);
    signal(SIGALRM, signal_handler);
    alarm(time_interval);
    while (processing) {
        printf(" waiting for you to INTERRUPT (cntrl-C) ...\n");
        sleep(1);
    }
    /* end while forever loop */
    return 0;
}

static void signal_handler(int sig) {
    switch (sig) {
        case SIGINT:    /* process for interrupt */
            longjmp(env, sig);
            /* break never reached */
        case SIGALRM:   /* process for alarm */
            longjmp(env, sig);
            /* break never reached */
        default:
            processing = 0;
            //exit(sig);
    }
}