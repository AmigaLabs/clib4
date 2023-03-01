#include <stdio.h>
#include <signal.h>

int main() {
    sigjmp_buf sj;
    sigset_t m;

    sigemptyset(&m);
    sigprocmask(SIG_SETMASK, &m, NULL);
    if (sigsetjmp(sj, 0) == 0) {
        sigaddset(&m, SIGUSR1);
        sigprocmask(SIG_SETMASK, &m, NULL);
        siglongjmp(sj, 1);
        return -1;
    }
    sigprocmask(SIG_SETMASK, NULL, &m);
    if (sigismember(&m, SIGUSR1))
        printf("SUCCESS\n");
    else
        printf("EXIT_FAILURE\n");
    return 0;
}