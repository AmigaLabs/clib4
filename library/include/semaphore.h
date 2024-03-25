/*
  $Id: semaphore.h,v 1.01 2022-07-18 12:09:49 clib4devs Exp $

  Copyright (C) 2015 Szilard Biro

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <pthread.h>
#include <time.h>
#include <limits.h>
#include <features.h>

#undef _POSIX_SEMAPHORES
#define _POSIX_SEMAPHORES

#undef SEM_VALUE_MAX
#define SEM_VALUE_MAX INT_MAX

#undef SEM_NSEMS_MAX
#define SEM_NSEMS_MAX 256

#undef SEM_FAILED
#define SEM_FAILED ((sem_t *)(-1))

struct _Node {
    struct _Node *ln_Succ; /* Pointer to next (successor) */
    struct _Node *ln_Pred; /* Pointer to previous (predecessor) */
    uint8_t       ln_Type;
    int8_t        ln_Pri;  /* Priority, for sorting */
    char         *ln_Name; /* ID string, null terminated */
};

struct sema {
    struct _Node node;
    int value;
    int waiters_count;
    pthread_mutex_t lock;
    pthread_cond_t count_nonzero;
};

typedef struct sema sem_t;

__BEGIN_DECLS

extern int sem_init(sem_t *sem, int pshared, unsigned int value);
extern int sem_destroy(sem_t *sem);
extern int sem_trywait(sem_t *sem);
extern int sem_wait(sem_t *sem);
extern int sem_timedwait(sem_t *sem, const struct timespec *abstime);
extern int sem_post(sem_t *sem);
extern sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);
extern int sem_close(sem_t *sem);
extern int sem_unlink(const char *name);
extern int sem_getvalue(sem_t *sem, int *sval);

__END_DECLS

#endif
