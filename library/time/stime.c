#define _DEFAULT_SOURCE

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

int stime(const time_t *t) {
    struct timeval tv = {.tv_sec = *t, .tv_usec = 0};
    return settimeofday(&tv, (void *) 0);
}
