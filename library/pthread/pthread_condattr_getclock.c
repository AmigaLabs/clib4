/*
$Id: pthread_condattr_getclock.c,v 1.00 2025-10-31 12:09:49 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
pthread_condattr_getclock(const pthread_condattr_t *attrp, clockid_t *outp) {

	*outp = attrp->clock_type;

	return 0;
}