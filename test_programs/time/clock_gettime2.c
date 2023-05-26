#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int utc_system_timestamp(char[]);

int main(void) {
    char buf[31];
    utc_system_timestamp(buf);
    printf("%s\n", buf);
}

// Allocate exactly 31 bytes for buf
int utc_system_timestamp(char buf[]) {
    const int bufsize = 31;
    const int tmpsize = 21;
    struct timespec now;
    struct tm tm;
    int retval = clock_gettime(CLOCK_REALTIME, &now);
    gmtime_r(&now.tv_sec, &tm);
    strftime(buf, tmpsize, "%Y-%m-%dT%H:%M:%S.", &tm);
    sprintf(buf + tmpsize - 1, "%09luZ", now.tv_nsec);
    return retval;
}