/*
 * $Id: resource.h,v 1.2 2006-07-28 14:02:32 clib4devs Exp $
*/

#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

/****************************************************************************/

#include <sys/types.h> /* For the definition of rlim_t */
#include <sys/time.h>
#include <features.h>

__BEGIN_DECLS

#define RLIM_INFINITY 0xffffffffUL
#define RLIM_SAVED_MAX (RLIM_INFINITY - 1)
#define RLIM_SAVED_CUR (RLIM_INFINITY - 2)

#define RLIM_VMEM 1
#define RLIM_AS RLIM_VMEM
#define RLIM_CORE 2
#define RLIM_CPU 3
#define RLIM_DATA 4
#define RLIM_FSIZE 5
#define RLIM_NOFILE 6
#define RLIM_OFILE RLIMIT_NOFILE
#define RLIM_STACK 7

#define RLIMIT_NLIMITS  8               /* upper bound of RLIMIT_* defines */
#define RLIM_NLIMITS    RLIMIT_NLIMITS

#define RLIMIT_NOFILE RLIM_NOFILE /* max number of open files */
#define RLIMIT_CPU RLIM_CPU		  /* CPU time in sec */
#define RLIMIT_FSIZE RLIM_FSIZE	  /* Maximum filesize */
#define RLIMIT_DATA RLIM_DATA	  /* max data size */
#define RLIMIT_STACK RLIM_STACK	  /* max stack size */
#define RLIMIT_CORE RLIM_CORE	  /* max core file size */

struct rlimit
{
	rlim_t rlim_cur;
	rlim_t rlim_max;
};

struct rusage
{
	struct timeval ru_utime; /* user CPU time used */
	struct timeval ru_stime; /* system CPU time used */
	long ru_maxrss;			 /* maximum resident set size */
	long ru_ixrss;			 /* integral shared memory size */
	long ru_idrss;			 /* integral unshared data size */
	long ru_isrss;			 /* integral unshared stack size */
	long ru_minflt;			 /* page reclaims (soft page faults) */
	long ru_majflt;			 /* page faults (hard page faults) */
	long ru_nswap;			 /* swaps */
	long ru_inblock;		 /* block input operations */
	long ru_oublock;		 /* block output operations */
	long ru_msgsnd;			 /* IPC messages sent */
	long ru_msgrcv;			 /* IPC messages received */
	long ru_nsignals;		 /* signals received */
	long ru_nvcsw;			 /* voluntary context switches */
	long ru_nivcsw;			 /* involuntary context switches */
};

#define RUSAGE_SELF     0
#define RUSAGE_CHILDREN (-1)
#define RUSAGE_BOTH     (-2) /* sys_wait4() uses this */
#define RUSAGE_THREAD   1    /* only the calling thread */

#define PRIO_MIN (-127)
#define PRIO_MAX 127

#define PRIO_PROCESS 0
#define PRIO_PGRP    1
#define PRIO_USER    2

extern int getrlimit(int resource, struct rlimit *rlp);
extern int setrlimit(int resource, const struct rlimit *rlp);
extern int getrusage(int who, struct rusage *usage);

extern int getpriority (int which, unsigned int who);
extern int setpriority (int which, unsigned int who, int priority);

__END_DECLS

#endif /* _SYS_RESOURCE_H */
