/*
  $Id: pthread_barrierattr_setpshared.c,v 1.00 2023-04-16 12:09:49 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared) {
    if (pshared > 1U)
        return EINVAL;
    attr->pshared = pshared ? INT_MIN : 0;
    return 0;
}