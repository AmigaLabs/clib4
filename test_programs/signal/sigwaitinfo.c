/* 
 * sigwaitchild.c
 *
 * This is an example of a parent process that creates some child
 * processes and then waits for them to terminate.  The waiting is
 * done using sigwaitinfo().  When a child process terminates, the
 * SIGCHLD signal is set on the parent.  sigwaitinfo() will return
 * when the signal arrives.
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void signal_handler(int signo)
{
    // Signal handler just to ensure the signal is caught
    printf("Signal %d caught\n", signo);
}

int main(void)
{
	sigset_t set;
    struct sigaction sa;
    siginfo_t info;
    int sig;

    // Set up the signal handler
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        printf("sigaction\n");
        return EXIT_FAILURE;
    }

    // Initialize the signal set and add SIGUSR1
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    // Block SIGUSR1 to ensure it will be caught by sigwaitinfo
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        printf("sigprocmask\n");
        return EXIT_FAILURE;
    }

    // Send SIGUSR1 to the current process
    if (kill(getpid(), SIGUSR1) == -1) {
        printf("kill\n");
        return EXIT_FAILURE;
    }

	sig = sigwaitinfo(&set, &info);

	if (sig == -1) {
		printf("sigwaitinfo() failed\n");
	}
	else {
   		printf("Signal number: %d\n", info.si_signo);
		switch( sig ) {
			case SIGCHLD:
				// info.si_pid is pid of terminated process, it is not POSIX
				printf("A child terminated; pid = %d\n", info.si_pid);
				break;
			case SIGUSR1:
				printf("sigwaitinfo received signal %d successfully\n", sig);
				break;
			default:
				// Should not get here since we asked only for SIGCHLD
				printf("Unexpected signal: %d\n", info.si_signo);
				return EXIT_FAILURE;
 		}
	}

	 return EXIT_SUCCESS;
}
