/*
  $Id: pthread_condattr_getpshared.c,v 1.00 2023-04-16 12:09:49 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared) {
    if (attr == NULL || pshared == NULL)
        return EINVAL;

    *pshared = (int)((unsigned)attr->pshared >> 31);
    return 0;
}