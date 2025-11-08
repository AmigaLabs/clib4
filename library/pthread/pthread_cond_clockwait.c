/*
	$Id: pthread_cond_clockwait.c,v 1.00 2025-11-02 12:09:49 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#include "common.h"
#include "pthread.h"

int
pthread_cond_clockwait(pthread_cond_t *cond, pthread_mutex_t *mutex, clockid_t clock_id, const struct timespec *abstime) {
	if (cond != NULL && cond->condattr == NULL) {
		pthread_condattr_t attr;
		pthread_condattr_init(&attr);
		cond->condattr = &attr;
	}

	return pthread_cond_timedwait(cond, mutex, abstime);
}