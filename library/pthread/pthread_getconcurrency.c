/*
  $Id: pthread_getconcurrency.c,v 1.00 2023-04-16 12:09:49 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "common.h"

int
pthread_getconcurrency(void) {
    return _pthread_concur;
}