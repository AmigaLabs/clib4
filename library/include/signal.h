/*
 * $Id: signal.h,v 1.8 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <features.h>
#include <pthread.h>

#ifdef __cplusplus
#ifdef __USE_AMIGAOS_NAMESPACE__
#define pthread_attr_t AmigaOS::pthread_attr_t
#endif
#endif

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
#define SIGWINCH     28   /* window size changed */
#define SIGLOST      29    /* resource lost (eg, record-lock lost) */
#define SIGUSR1      30    /* user defined signal 1 */
#define SIGUSR2      31    /* user defined signal 2 */
#define NSIG         32       /* signal 0 implied */

/****************************************************************************/

extern _sig_func_ptr signal (int, _sig_func_ptr);;
extern int raise(int sig);

typedef int sig_atomic_t;

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif /* _SYS_TYPES_H */

typedef void (*sig_t)(int);

typedef int sigset_t;

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#define SI_ASYNCIO (-4)

extern int sigmask(int signum);
extern int sigblock(int signal_mask);
extern int sigsetmask(int signal_mask);
extern int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
extern int sigismember(const sigset_t *set, int sig);
extern int sigemptyset(sigset_t *set);
extern int sigfillset(sigset_t *set);
extern int sigdelset(sigset_t *set, int sig);
extern int sigaddset(sigset_t *set, int sig);
extern int kill(pid_t pid, int signal_number);

typedef struct {
    void *ss_sp;
    int ss_flags;
    size_t ss_size;
} stack_t;

/* Sigaction */
/* Unsupported flags are not present.  */
#define SA_RESETHAND 1
#define SA_NODEFER 2
#define SA_RESTART 4
#define SA_SIGINFO 8

#define sa_handler _sa_func._sa_handler
#define sa_sigaction _sa_func._sa_sigaction

union sigval {
    int sival_int;
    void *sival_ptr;
};

struct siginfo_t {
    int si_signo;
    int si_code;
    int si_errno;
    pid_t si_pid;
    uid_t si_uid;
    void *si_addr;
    int si_status;
    long si_band;
    union sigval si_value;
};
typedef struct siginfo_t siginfo_t;

struct sigaction {
    union {
        void (*_sa_handler)(int);

        /* Present to allow compilation, but unsupported by gnulib.  POSIX
           says that implementations may, but not must, make sa_sigaction
           overlap with sa_handler, but we know of no implementation where
           they do not overlap.  */
        void (*_sa_sigaction)(int, siginfo_t *, void *);
    } _sa_func;

    sigset_t sa_mask;
    /* Not all POSIX flags are supported.  */
    int sa_flags;
};

extern int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);

typedef union sigval sigval_t;


struct sigevent {
    union sigval sigev_value;
    int sigev_signo;
    int sigev_notify;
    union {
        char __pad[64 - 2*sizeof(int) - sizeof(union sigval)];
        pid_t sigev_notify_thread_id;
        struct {
            void (*sigev_notify_function)(union sigval);
            pthread_attr_t *sigev_notify_attributes;
        } __sev_thread;
    } __sev_fields;
};

#define sigev_notify_thread_id __sev_fields.sigev_notify_thread_id
#define sigev_notify_function __sev_fields.__sev_thread.sigev_notify_function
#define sigev_notify_attributes __sev_fields.__sev_thread.sigev_notify_attributes

#define SIGEV_SIGNAL 0
#define SIGEV_NONE 1
#define SIGEV_THREAD 2
#define SIGEV_THREAD_ID 4

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define MINSIGSTKSZ 4096
#define SIGSTKSZ 10240
#endif

__END_DECLS

#endif /* _SIGNAL_H */
