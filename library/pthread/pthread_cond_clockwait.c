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
	if (cond == NULL || mutex == NULL || abstime == NULL)
		return EINVAL;

	if (clock_id != CLOCK_REALTIME && clock_id != CLOCK_MONOTONIC)
		return EINVAL;

	/* Temporarily override the clock type for this wait, then restore it */
	clockid_t old_clock = cond->condattr.clock_type;
	cond->condattr.clock_type = clock_id;

	int ret = _pthread_cond_timedwait(cond, mutex, abstime, FALSE);

	cond->condattr.clock_type = old_clock;

	return ret;
}