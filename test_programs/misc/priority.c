#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>

int
main(int argc, char *argv[]) {
    pid_t pid = getpid();  // Get current process ID
    int which = PRIO_PROCESS;
    int priority;

    // Get the current priority
    errno = 0;  // Clear errno before calling getpriority
    priority = getpriority(which, pid);
    if (priority == -1 && errno != 0) {
        perror("getpriority");
        exit(EXIT_FAILURE);
    }

    printf("Current priority of process %d: %d\n", pid, priority);

    // Set a new priority (nice value)
    int new_priority = 10;
    if (setpriority(which, pid, new_priority) == -1) {
        perror("setpriority");
        exit(EXIT_FAILURE);
    }

    // Confirm the priority was changed
    errno = 0;
    priority = getpriority(which, pid);
    if (priority == -1 && errno != 0) {
        perror("getpriority");
        exit(EXIT_FAILURE);
    }

    printf("New priority of process %d: %d\n", pid, priority);

    return 0;
}
