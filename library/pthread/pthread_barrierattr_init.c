/*
  $Id: pthread_barrier_init.c,v 1.00 2023-04-16 12:09:49 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_barrierattr_init(pthread_barrierattr_t *attr) {
    *attr = (pthread_barrierattr_t) {0};

    return 0;
}