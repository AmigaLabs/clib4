/*
  Copyright (C) 2014 Szilard Biro
  Copyright (C) 2018 Harry Sintonen
  Copyright (C) 2023 clib4Devs

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

#ifndef PTHREAD_H
#define PTHREAD_H

#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <sched.h>
#include <features.h>

__BEGIN_DECLS

//
// POSIX options
//

#undef _POSIX_THREADS
#define _POSIX_THREADS
#undef _POSIX_READER_WRITER_LOCKS
#define _POSIX_READER_WRITER_LOCKS
#undef _POSIX_SPIN_LOCKS
#define _POSIX_SPIN_LOCKS
#undef _POSIX_THREAD_ATTR_STACKSIZE
#define _POSIX_THREAD_ATTR_STACKSIZE
#undef _POSIX_THREAD_PRIORITY_SCHEDULING
#define _POSIX_THREAD_PRIORITY_SCHEDULING

//
// POSIX limits
//

#define PTHREAD_KEYS_MAX                      64
#define PTHREAD_STACK_MIN                     65536
#define PTHREAD_THREADS_MAX                   2019
#define PTHREAD_DESTRUCTOR_ITERATIONS         4

//
// POSIX pthread types
//

typedef unsigned int pthread_t;
typedef unsigned int pthread_key_t;

//
// POSIX thread attribute values
//
#ifndef PTHREAD_CREATE_JOINABLE
#define PTHREAD_CREATE_JOINABLE       0
#endif

#ifndef PTHREAD_CREATE_DETACHED
#define PTHREAD_CREATE_DETACHED       1
#endif

#ifndef PTHREAD_INHERIT_SCHED
#define PTHREAD_INHERIT_SCHED         0
#endif

#ifndef PTHREAD_EXPLICIT_SCHED
#define PTHREAD_EXPLICIT_SCHED        1
#endif

#define PTHREAD_SCOPE_PROCESS         0
#define PTHREAD_SCOPE_SYSTEM          1

#define PTHREAD_CANCEL_ENABLE         0
#define PTHREAD_CANCEL_DISABLE        1

#define PTHREAD_CANCEL_ASYNCHRONOUS   0
#define PTHREAD_CANCEL_DEFERRED       1

#define PTHREAD_PROCESS_PRIVATE       0
#define PTHREAD_PROCESS_SHARED        1

//
// Threads
//

struct pthread_attr {
    void *stackaddr;
    size_t stacksize;
    int detachstate;
    struct sched_param param;
    int inheritsched;
    int contentionscope;
    size_t guardsize;
    int policy;
};

typedef struct pthread_attr pthread_attr_t;

#define PTHREAD_CANCELED              ((void *)-1)

//
// Once key
//

struct pthread_once {
    volatile int done;
    int started;
    int lock;
};

typedef struct pthread_once pthread_once_t;

#define PTHREAD_ONCE_INIT       {0, -1, 0}

//
// Mutex
//

#define PTHREAD_MUTEX_NORMAL     0
#define PTHREAD_MUTEX_RECURSIVE  1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT    PTHREAD_MUTEX_NORMAL

struct pthread_mutexattr {
    int pshared;
    int kind;
};

typedef struct pthread_mutexattr pthread_mutexattr_t;

struct pthread_mutex;

typedef struct pthread_mutex pthread_mutex_t;

#define NULL_MINLIST {0, 0, 0}
#define NULL_MINNODE {0, 0}
#define NULL_NODE {0, 0, 0, 0, 0}
#define NULL_SEMAPHOREREQUEST {NULL_MINNODE, 0}
#define NULL_SEMAPHORE {NULL_NODE, 0, NULL_MINLIST, NULL_SEMAPHOREREQUEST, 0, 0}

#define PTHREAD_MUTEX_INITIALIZER {NULL, PTHREAD_MUTEX_NORMAL, 0, NULL}
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER {NULL, PTHREAD_MUTEX_RECURSIVE, 0, NULL}
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER {NULL, PTHREAD_MUTEX_ERRORCHECK, 0, NULL}

//
// Condition variables
//

struct pthread_condattr {
    int pshared;
};

typedef struct pthread_condattr pthread_condattr_t;

struct pthread_cond;

typedef struct pthread_cond pthread_cond_t;

#define PTHREAD_COND_INITIALIZER {0, NULL_SEMAPHORE, NULL_MINLIST}

//
// Barriers
//

#define PTHREAD_BARRIER_SERIAL_THREAD 1

struct pthread_barrierattr {
    int pshared;
};

typedef struct pthread_barrierattr pthread_barrierattr_t;

struct pthread_barrier;

typedef struct pthread_barrier pthread_barrier_t;

//
// Read-write locks
//

struct pthread_rwlockattr {
    int pshared;
};

typedef struct pthread_rwlockattr pthread_rwlockattr_t;

struct pthread_rwlock;

typedef struct pthread_rwlock pthread_rwlock_t;

#define PTHREAD_RWLOCK_INITIALIZER {NULL_SEMAPHORE}

//
// Spinlocks
//

typedef int pthread_spinlock_t;

#define PTHREAD_SPINLOCK_INITIALIZER 0

//
// POSIX thread routines
//

//
// Thread attribute functions
//

extern int pthread_attr_init(pthread_attr_t *attr);
extern int pthread_attr_destroy(pthread_attr_t *attr);
extern int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
extern int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
extern int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);
extern int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
extern int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
extern int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
extern int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);
extern int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
extern int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
extern int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
extern int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);
extern int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
extern int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);
extern int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);

extern int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
extern int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
//
// Thread functions
//

extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg);
extern int pthread_detach(pthread_t thread);
extern int pthread_equal(pthread_t t1, pthread_t t2);
extern void pthread_exit(void *value_ptr);
extern int pthread_join(pthread_t thread, void **value_ptr);
extern pthread_t pthread_self(void);
extern int pthread_cancel(pthread_t thread);
extern int pthread_setcancelstate(int state, int *oldstate);
extern int pthread_setcanceltype(int type, int *oldtype);
extern void pthread_testcancel(void);
extern int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));
extern int pthread_num_processors_np (void);

#define pthread_sigmask(blocktype, mask_ptr, unused1) sigprocmask(blocktype, mask_ptr, unused1)

//
// Scheduling functions
//

extern int pthread_setschedprio(pthread_t thread, int prio);
extern int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);
extern int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param);
extern int pthread_setconcurrency(int level);
extern int pthread_getconcurrency(void);

//
// Thread specific data functions
//

extern int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
extern int pthread_key_delete(pthread_key_t key);
extern int pthread_setspecific(pthread_key_t key, const void *value);
extern void *pthread_getspecific(pthread_key_t key);

//
// Mutex attribute functions
//

extern int pthread_mutexattr_init(pthread_mutexattr_t *attr);
extern int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
extern int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared);
extern int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
extern int pthread_mutexattr_gettype(pthread_mutexattr_t *attr, int *kind);
extern int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind);

//
// Mutex functions
//

extern int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
extern int pthread_mutex_destroy(pthread_mutex_t *mutex);
extern int pthread_mutex_lock(pthread_mutex_t *mutex);
extern int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);
extern int pthread_mutex_trylock(pthread_mutex_t *mutex);
extern int pthread_mutex_unlock(pthread_mutex_t *mutex);

//
// Condition variable attribute functions
//

extern int pthread_condattr_init(pthread_condattr_t *attr);
extern int pthread_condattr_destroy(pthread_condattr_t *attr);
extern int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);
extern int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);

//
// Condition variable functions
//

extern int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
extern int pthread_cond_destroy(pthread_cond_t *cond);
extern int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
extern int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
extern int pthread_cond_signal(pthread_cond_t *cond);
extern int pthread_cond_broadcast(pthread_cond_t *cond);

//
// Barrier attribute functions
//

extern int pthread_barrierattr_init(pthread_barrierattr_t *attr);
extern int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
extern int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr, int *pshared);
extern int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);

//
// Barrier functions
//

extern int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);
extern int pthread_barrier_destroy(pthread_barrier_t *barrier);
extern int pthread_barrier_wait(pthread_barrier_t *barrier);

//
// Read-write lock attribute functions
//

extern int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
extern int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
extern int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr, int *pshared);
extern int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

//
// Read-write lock functions
//

extern int pthread_rwlock_init(pthread_rwlock_t *lock, const pthread_rwlockattr_t *attr);
extern int pthread_rwlock_destroy(pthread_rwlock_t *lock);
extern int pthread_rwlock_tryrdlock(pthread_rwlock_t *lock);
extern int pthread_rwlock_trywrlock(pthread_rwlock_t *lock);
extern int pthread_rwlock_rdlock(pthread_rwlock_t *lock);
extern int pthread_rwlock_timedrdlock(pthread_rwlock_t *lock, const struct timespec *abstime);
extern int pthread_rwlock_wrlock(pthread_rwlock_t *lock);
extern int pthread_rwlock_timedwrlock(pthread_rwlock_t *lock, const struct timespec *abstime);
extern int pthread_rwlock_unlock(pthread_rwlock_t *lock);

//
// Spinlock functions
//

extern int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
extern int pthread_spin_destroy(pthread_spinlock_t *lock);
extern int pthread_spin_lock(pthread_spinlock_t *lock);
extern int pthread_spin_trylock(pthread_spinlock_t *lock);
extern int pthread_spin_unlock(pthread_spinlock_t *lock);

//
// Non-portable functions
//

extern int pthread_setname_np(pthread_t thread, const char *name);
extern int pthread_getname_np(pthread_t thread, char *name, size_t len);
extern int pthread_cond_timedwait_relative_np(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *reltime);
extern int pthread_getattr_np(pthread_t thread, pthread_attr_t *attr);

//
// Cancellation cleanup
//

extern void pthread_cleanup_push(void (*routine)(void *), void *arg);
extern void pthread_cleanup_pop(int execute);

//
// Signalling
//

extern int pthread_kill(pthread_t thread, int sig);

__END_DECLS

#endif
