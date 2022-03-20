/*
 * $Id: semaphore.h,v 1.00 2021-01-17 12:09:49 clib2devs Exp $
*/

#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <sys/cdefs.h>
#include <stdlib.h>

__BEGIN_DECLS

typedef void *sem_t;

extern int sem_init(sem_t *sem, int pshared, unsigned int value);
extern int sem_destroy(sem_t *sem);
extern int sem_trywait(sem_t *sem);
extern int sem_wait(sem_t *sem);
extern int sem_timedwait(sem_t *sem, const struct timespec *timeout);
extern int sem_post(sem_t *sem);
extern int sem_getvalue(sem_t *sem, int *sval);

__END_DECLS

#endif
