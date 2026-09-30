#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int i = 0;

struct child_to_clean {
    pid_t pid;
    struct child_process *process;
    struct child_to_clean *next;
};

static struct child_to_clean *children_to_clean;

void f1(void) {
    printf("[f1] i=%d\n", i);
}

void f2(void) {
    printf("[f2] i=%d\n", i);
}

void f3(void) {
    printf("[f3] i=%d\n", i);
}

int main(void) {
    i = 1;
    if (atexit(f1) || atexit(f2) || atexit(f3)) {
        // atexit registration failed
        return EXIT_FAILURE;
    }

    /* Loop printing until CTRL-C: the atexit handlers must run on break
       (expected output on CTRL-C: f3, f2, f1). */
    printf("Press CTRL-C to stop; atexit handlers should run\n");
    for (;;) {
        printf("[main] i=%d\n", i++);
        sleep(1);
    }

    return EXIT_SUCCESS;
}
