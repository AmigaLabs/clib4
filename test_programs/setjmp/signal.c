#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

sigjmp_buf jmpbuf;
volatile sig_atomic_t canjump;

void pr_mask(const char *str) {
    sigset_t sigset;
    int errno_save;

    // Backup the errno value since this function may change it
    errno_save = errno;

    // Get the current signal mask
    if (sigprocmask(0, NULL, &sigset) < 0) {
        printf("sigprocmask error!\n");
        exit(1);
    }

    printf("%s", str);

    // Output the signal mask
    if (sigismember(&sigset, SIGINT)) printf("SIGINT ");
    if (sigismember(&sigset, SIGQUIT)) printf("SIGQUIT ");
    if (sigismember(&sigset, SIGUSR1)) printf("SIGUSR1 ");
    if (sigismember(&sigset, SIGALRM)) printf("SIGALRM ");

    printf("\n");

    //Restore the errno value
    errno = errno_save;
}

void sig_usr1(int signo) {
    time_t starttime;

    // Precheck the canjump variable
    if (canjump == 0)
        return;

    pr_mask("starting sig_usr1: ");

    // Setup alarm for 3 seconds and let program wait for 5 seconds
    // During this period, sig_alrm will start running, and then return back
    alarm(3);
    starttime = time(NULL);
    for (;;)
        if (time(NULL) > starttime + 5) break;
    pr_mask("finishing sig_usr1: ");

    // Turn off the canjump, to skip future SIGUSR1 signal
    canjump = 0;
    siglongjmp(jmpbuf, 1);
}

void sig_alrm(int signo) {
    pr_mask("in sig_alrm: ");
}

int main(int argc, char *argv[]) {
    // Setup the signal handler for SIGUSR1, SIGALRM
    if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
        printf("signal error!\n");
        exit(1);
    }

    if (signal(SIGALRM, sig_alrm) == SIG_ERR) {
        printf("signal error!\n");
        exit(2);
    }

    pr_mask("starting main: ");

    // Setup the signal jump entry
    if (sigsetjmp(jmpbuf, 1)) {
        pr_mask("ending main: ");
        exit(0);
    }

    // Note: the reason to setump canjump after "sigsetjmp" here is to prevent
    // the SIGUSR1 signal come before sigssetjmp. Please keep in mind that,
    // signal can come at any time to interrupt the normal codeflow. And the
    // program need to prepare for that.
    canjump = 1;

    // Pause function to wait for signal coming
    for (;;)
        pause();

    exit(0);
}