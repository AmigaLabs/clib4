/*
  $Id: pthread_attr_setinheritsched.c,v 1.00 2023-04-16 12:09:49 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_attr_setinheritsched(pthread_attr_t *attr, int inherit) {
    if (inherit > 1U)
        return EINVAL;
    attr->inheritsched = inherit;
    return 0;
}