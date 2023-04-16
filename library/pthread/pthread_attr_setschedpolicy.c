/*
  $Id: pthread_attr_setschedpolicy.c,v 1.00 2023-04-16 12:09:49 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) {
    attr->policy = policy;
    return 0;
}
