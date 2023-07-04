/*
 * $Id: sem.h,v 1.00 2021-02-02 17:03:49 clib2devs Exp $
*/

#ifndef _SYS_SEM_H
#define _SYS_SEM_H

#include <features.h>
/* Get common definition of System V style IPC.  */
#include <sys/ipc.h>
#include <time.h>

/* The following System V style IPC functions implement a semaphore
   handling.  The definition is found in XPG2.  */

/* Structure used for argument to `semop' to describe operations.  */
struct sembuf
{
    unsigned short int sem_num; /* semaphore number */
    short int sem_op;           /* semaphore operation */
    short int sem_flg;          /* operation flag */
};

__BEGIN_DECLS

/* Semaphore control operation.  */
extern int _semctl(int semid, int semnum, int cmd, union semun aun);
/* Get semaphore.  */
extern int _semget(key_t key, int nsems, int flags);
/* Operate on semaphore.  */
extern int _semop(int semid, const struct sembuf *ops, int nops);
extern long int _semids(long int *buf, uint32_t nids, long unsigned int *idcnt);
#ifdef __USE_GNU
/* Operate on semaphore with timeout.  */
extern int _semtimedop(int semid, const struct sembuf *ops, int nops, struct timespec *to);
#endif

#define semctl(a, b, c, d)     _semctl(a, b, c, d)
#define semget(a, b, c)        _semget(a, b, c)
#define semop(a, b, c)         _semop(a, b, c)
#define semids(a, b, c)        _semids(a, b, c)
#ifdef __USE_GNU
#undef semtimedop
#define semtimedop(a, b, c, d) _semtimedop(a, b, c, d)
#endif

__END_DECLS

#endif /* sys/sem.h */