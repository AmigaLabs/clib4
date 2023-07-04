/*
  $Id: pthread_attr_getschedpolicy.c,v 1.00 2023-04-16 12:09:49 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy) {
    *policy = attr->policy;
    return 0;
}