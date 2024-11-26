#include <signal.h>

int
main() {
    sigset_t ss;
    struct sigaction sa;

    sigemptyset(&ss);
    sigsuspend(&ss);
    sigaction(SIGINT, &sa, (struct sigaction *) 0);
    sigprocmask(SIG_BLOCK, &ss, (sigset_t *) 0);

    return 0;
}