// C program to illustrate default Signal Handler
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int loop = 1;
// Handler for SIGINT, caused by
// Ctrl-C at keyboard
void handle_sigint(int sig) {
    printf("Caught signal %s\n", strsignal(sig));
    loop = 0;
}

int main() {
    signal(SIGINT, handle_sigint);
    while (loop) {
        printf("Hello world\n");
        sleep(1);
    }
    printf("bye bye\n");

    return 0;
}