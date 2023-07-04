/*
  $Id: pthread_attr_getinheritsched.c,v 1.00 2023-04-16 12:09:49 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inherit) {
    *inherit = attr->inheritsched;
    return 0;
}