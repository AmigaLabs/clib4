/*
  $Id: pthread_attr_getguardsize.c,v 1.00 2023-03-03 12:09:49 clib4devs Exp $
*/

#include "pthread.h"

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize) {
    if (attr == NULL)
        return EINVAL;

    if (guardsize != NULL)
        *guardsize = attr->guardsize;

    return 0;
}