#include <stdio.h>
#include <string.h>
#include <time.h>

int main(void) {
    char tmbuf[64], buf[64];
    time_t nowtime;
    struct tm *nowtm;
    struct timeval tv1;
    struct timezone tz;

    gettimeofday(&tv1, &tz);
    nowtime = tv1.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
    snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, tv1.tv_usec);
    printf("%s\n", buf);

    return 0;
}