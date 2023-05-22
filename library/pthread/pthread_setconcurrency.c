/*
  $Id: pthread_setconcurrency.c,v 1.00 2023-04-16 12:09:49 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "common.h"

int
pthread_setconcurrency(int val) {
    if (val < 0)
        return EINVAL;
    if (val > 0)
        return EAGAIN;
    _pthread_concur = val;
    return 0;
}