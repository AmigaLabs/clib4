/*
 * $Id: time_srdtsc.c,v 1.1 2023-02-22 18:06:24 clib4devs Exp $
 */

#include <time.h>
#include <sys/time.h>

unsigned long long rdtsc(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}