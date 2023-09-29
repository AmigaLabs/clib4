/*
 * $Id: stat_futimens.c,v 1.0 2023-05-26 12:04:24 clib4devs Exp $
*/

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

int futimens(int fd, const struct timespec times[2]) {
    return utimensat(fd, 0, times, 0);
}