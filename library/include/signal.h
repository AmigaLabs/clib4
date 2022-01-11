/*
 * $Id: signal.h,v 1.8 2006-01-08 12:06:14 obarthel Exp $
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

#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <features.h>

__BEGIN_DECLS

typedef void (*_sig_func_ptr)(int);

typedef int sig_atomic_t;           /* Atomic entity type (ANSI) */
#ifndef _POSIX_SOURCE
typedef _sig_func_ptr sig_t;        /* BSD naming */
typedef _sig_func_ptr sighandler_t; /* glibc naming */
#endif                              /* !_POSIX_SOURCE */

#define SIG_DFL ((_sig_func_ptr)0)  /* Default action */
#define SIG_IGN ((_sig_func_ptr)1)  /* Ignore action */
#define SIG_ERR ((_sig_func_ptr)-1) /* Error return */

/****************************************************************************/

/* TO IMPLEMENT INTO LIBRARY (if possible) */
#define SIGHUP       1   /* hangup */
#define SIGINT       2   /* interrupt */
#define SIGQUIT      3  /* quit */
#define SIGILL       4   /* illegal instruction (not reset when caught) */
#define SIGTRAP      5  /* trace trap (not reset when caught) */
#define SIGABRT      6  /* used by abort, replace SIGIOT in the future */
#define SIGIOT       SIGABRT   /* IOT instruction */
#define SIGEMT       7   /* EMT instruction */
#define SIGFPE       8   /* floating point exception */
#define SIGKILL      9  /* kill (cannot be caught or ignored) */
#define SIGBUS       10  /* bus error */
#define SIGSEGV      11 /* segmentation violation */
#define SIGSYS       12  /* bad argument to system call */
#define SIGPIPE      13 /* write on a pipe with no one to read it */
#define SIGALRM      14 /* alarm clock */
#define SIGTERM      15 /* software termination signal from kill */
#define SIGURG       16     /* urgent condition on IO channel */
#define SIGSTOP      17    /* sendable stop signal not from tty */
#define SIGTSTP      18    /* stop signal from tty */
#define SIGCONT      19    /* continue a stopped process */
#define SIGCHLD      20    /* to parent on child stop or exit */
#define SIGCLD       SIGCHLD     /* System V name for SIGCHLD */
#define SIGTTIN      21    /* to readers pgrp upon background tty read */
#define SIGTTOU      22    /* like TTIN for output if (tp->t_local&LTOSTOP) */
#define SIGIO        23      /* input/output possible signal */
#define SIGPOLL      SIGIO /* System V name for SIGIO */
#define SIGXCPU      24    /* exceeded CPU time limit */
#define SIGXFSZ      25    /* exceeded file size limit */
#define SIGVTALRM    26  /* virtual time alarm */
#define SIGPROF      27    /* profiling time alarm */
#define SIGWINCH     28   /* window changed */
#define SIGLOST      29    /* resource lost (eg, record-lock lost) */
#define SIGUSR1      30    /* user defined signal 1 */
#define SIGUSR2      31    /* user defined signal 2 */
#define NSIG         32       /* signal 0 implied */

/****************************************************************************/

extern void (*signal(int sig, void (*)(int)))(int);
extern int raise(int sig);

/****************************************************************************/

/* The following is not part of the ISO 'C' (1994) standard, but it should
   be part of ISO/IEC 9899:1999, also known as "C99". */

/****************************************************************************/

typedef int sig_atomic_t;

/****************************************************************************/

/* The following is not part of the ISO 'C' (1994) standard. */

/****************************************************************************/

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif /* _SYS_TYPES_H */

/****************************************************************************/

typedef void (*sig_t)(int);

/****************************************************************************/

typedef int sigset_t;

/****************************************************************************/

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

/****************************************************************************/

extern int sigmask(int signum);
extern int sigblock(int signal_mask);
extern int sigsetmask(int signal_mask);
extern int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
extern int sigemptyset(sigset_t *set);
extern int sigaddset(sigset_t *set, int sig);
extern int kill(pid_t pid, int signal_number);

__END_DECLS

#endif /* _SIGNAL_H */
