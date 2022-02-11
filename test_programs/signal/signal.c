// CPP program to illustrate
// default Signal Handler
#include <stdio.h>
#include <signal.h>

int loop = 1;
// Handler for SIGINT, caused by
// Ctrl-C at keyboard
void handle_sigint(int sig) {
    printf("Caught signal %d\n", sig);
    loop = 0;
}

int main() {
    signal(SIGINT, handle_sigint);
    while (loop) {
        printf("Hello world\n");
        sleep(1);
    }
    return 0;
}