/*
 * $Id: stdio_lock.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

static struct SignalSemaphore * stdio_lock;

void
__stdio_lock(void)
{
	assert( stdio_lock != NULL );

	if(stdio_lock != NULL)
		ObtainSemaphore(stdio_lock);
}

void
__stdio_unlock(void)
{
	assert( stdio_lock != NULL );

	if(stdio_lock != NULL)
		ReleaseSemaphore(stdio_lock);
}

void
__stdio_lock_exit(void)
{
	__delete_semaphore(stdio_lock);
	stdio_lock = NULL;
}

int
__stdio_lock_init(void)
{
	int result = ERROR;

	stdio_lock = __create_semaphore();
	if(stdio_lock == NULL)
		goto out;

	result = OK;

 out:

	return(result);
}
