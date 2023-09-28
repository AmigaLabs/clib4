/*
  $Id: pthread_attr_setguardsize.c,v 1.00 2023-03-03 12:09:49 clib4devs Exp $
*/

#include "pthread.h"

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
    if (attr == NULL || guardsize < 0)
        return EINVAL;

    attr->guardsize = guardsize;

    return 0;
}