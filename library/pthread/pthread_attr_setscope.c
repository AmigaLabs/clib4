/*
  $Id: pthread_attr_setscope.c,v 1.00 2023-04-16 12:09:49 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_attr_setscope(pthread_attr_t *attr, int scope) {
    switch (scope) {
        case PTHREAD_SCOPE_SYSTEM:
            return 0;
        case PTHREAD_SCOPE_PROCESS:
            return ENOTSUP;
        default:
            return EINVAL;
    }
}