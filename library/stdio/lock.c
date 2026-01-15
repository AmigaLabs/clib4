/*
 * $Id: stdio_lock.c,v 1.5 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

static int SemaphoreIsMine(struct SignalSemaphore *sem) {
	struct Task *me = FindTask(NULL);
	return (sem && sem->ss_NestCount > 0 && sem->ss_Owner == me);
}

#ifdef DEBUG_LOCKS
void
__stdio_lock_real(struct _clib4 *__clib4) {
    if (__clib4->stdio_lock != NULL)
        ObtainSemaphoreShared(__clib4->stdio_lock);
}

void
__stdio_unlock_real(struct _clib4 *__clib4) {
	if (__clib4->stdio_lock != NULL && SemaphoreIsMine(__clib4->stdio_lock))
		ReleaseSemaphore(__clib4->stdio_lock);
}

void __stdio_lock_special(char const *caller_name, struct _clib4 *__clib4)
{
	D(( "__stdio_lock was called from %s and thread %x\n", caller_name, FindTask(NULL) ));
	__stdio_lock_real(__clib4);
}

void __stdio_unlock_special(char const *caller_name, struct _clib4 *__clib4)
{
	D(( "__stdio_unlock was called from %s and thread %x\n", caller_name, FindTask(NULL) ));
	__stdio_unlock_real(__clib4);
}
#else
void
__stdio_lock(struct _clib4 *__clib4) {
	if (__clib4->stdio_lock != NULL)
		ObtainSemaphoreShared(__clib4->stdio_lock);
}

void
__stdio_unlock(struct _clib4 *__clib4) {
	if (__clib4->stdio_lock != NULL && SemaphoreIsMine(__clib4->stdio_lock))
		ReleaseSemaphore(__clib4->stdio_lock);
}
#endif

void
__stdio_lock_exit(struct _clib4 *__clib4) {
    __delete_semaphore(__clib4->stdio_lock);
    __clib4->stdio_lock = NULL;
}

int
__stdio_lock_init(struct _clib4 *__clib4) {
    int result = ERROR;

    __clib4->stdio_lock = __create_semaphore();
    if (__clib4->stdio_lock == NULL)
        goto out;

    result = OK;
out:

    return result;
}
