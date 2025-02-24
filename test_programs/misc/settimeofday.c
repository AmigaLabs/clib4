#include <sys/time.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    struct timeval now;
    int rc;

    now.tv_sec = 866208142;
    now.tv_usec = 290944;

    rc = settimeofday(&now, NULL);
    if (rc == 0) {
        printf("settimeofday() successful.\n");
    } else {
        printf("settimeofday() failed, errno = %d\n", errno);
        return -1;
    }

    return 0;
}