#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    // Get the TZ environment variable
    char *tz = getenv("TZ");
    if (tz == NULL) {
        printf("TZ is not set\n");
    } else {
        printf("TZ is set to: %s\n", tz);
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    if (local == NULL) {
        perror("localtime");
        return 1;
    }

    // Print the local time
    printf("Local time: %s", asctime(local));

    // Set the TZ environment variable
    if (setenv("TZ", "PST8PDT", 1) != 0) {
        perror("setenv");
        return 1;
    }

    // Get the TZ environment variable
    tz = getenv("TZ");
    if (tz == NULL) {
        printf("TZ is not set\n");
    } else {
        printf("TZ is set to: %s\n", tz);
    }

    // Get the current time
    now = time(NULL);
    local = localtime(&now);
    if (local == NULL) {
        perror("localtime");
        return 1;
    }

    // Print the local time
    printf("Local time: %s", asctime(local));

    return 0;
}