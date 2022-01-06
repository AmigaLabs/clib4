/*
 * $Id: sem.h,v 1.00 2021-02-02 17:03:49 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 */

#ifndef _SYS_SEM_H
#define _SYS_SEM_H

#include <features.h>
/* Get common definition of System V style IPC.  */
#include <sys/ipc.h>

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
#ifdef __USE_GNU
/* Operate on semaphore with timeout.  */
extern int _semtimedop(int semid, const struct sembuf *ops, int nops, struct timespec *to);
#endif

#define semctl(a, b, c, d)     _semctl(a, b, c, d)
#define semget(a, b, c)        _semget(a, b, c)
#define semop(a, b, c)         _semop(a, b, c)
#ifdef __USE_GNU
#define semtimedop(a, b, c, d) _semtimedop(a, b, c, d)
#endif

__END_DECLS

#endif /* sys/sem.h */