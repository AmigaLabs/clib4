#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void foo() {
    void *callstack[10];
    int frames = backtrace(callstack, 10);

    printf("Backtrace (%d frames):\n", frames);
    for (int i = 0; i < frames; i++) {
        printf("#%d %p\n", i, callstack[i]);
    }

    // Print symbols to stdout (fd=1)
    printf("\nbacktrace_symbols_fd:\n");
    backtrace_symbols_fd(callstack, frames, 1); // 1 = stdout

    // Get symbols as strings
    printf("\nbacktrace_symbols:\n");
    char **symbols = backtrace_symbols(callstack, frames);
    for (int i = 0; i < frames; i++) {
        printf("%s\n", symbols[i]);
        free(symbols[i]); // Free each string
    }
    free(symbols); // Free the array
}

void bar() {
    foo();
}

int main() {
    bar();
    return 0;
}
