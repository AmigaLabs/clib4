/*
 * $Id: resource.h,v 1.2 2006-07-28 14:02:32 obarthel Exp $
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

extern int getrlimit(int resource, struct rlimit *rlp);
extern int setrlimit(int resource, const struct rlimit *rlp);
extern int getrusage(int who, struct rusage *usage);

__END_DECLS

#endif /* _SYS_RESOURCE_H */
