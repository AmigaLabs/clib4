/*
  $Id: pthread_barrierattr_getpshared.c,v 1.00 2023-04-16 12:09:49 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared) {
    *pshared = !!attr->pshared;
    return 0;
}