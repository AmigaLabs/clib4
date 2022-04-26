/*
 * $Id: timeb_ftime.c,v 1.2 2006-07-28 14:37:27 clib2devs Exp $
*/

#include <sys/timeb.h>
#include <sys/time.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
ftime(struct timeb *tb) {
    struct timeval tv;
    struct timezone tz;
    int retval = -1;

    ENTER();

    if (tb == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    if (gettimeofday(&tv, &tz) != 0)
        goto out;

    tb->time = tv.tv_sec;
    tb->millitm = tv.tv_usec / 1000;
    tb->timezone = tz.tz_minuteswest;
    tb->dstflag = tz.tz_dsttime;

    retval = 0;

out:

    RETURN(retval);
    return (retval);
}
