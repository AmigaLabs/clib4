/*
  $Id: pthread_condattr_getpshared.c,v 1.00 2023-04-16 12:09:49 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared) {
    *pshared = attr->pshared >> 31;
    return 0;
}