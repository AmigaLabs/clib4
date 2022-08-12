#include <stdio.h>
#include <string.h>
#include <signal.h>

int main() {
    for (int i = 0; i < NSIG; i++) {
        printf("Signal %d = %s\n", i, strsignal(i));
    }
    return 0;
}