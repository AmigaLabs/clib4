/*
 * $Id: sched.h,v 1.2 2021-01-14 12:06:14 clib2devs Exp $
*/

#ifndef _SCHED_H
#define _SCHED_H

#include <features.h>

__BEGIN_DECLS

#define SCHED_OTHER 0
#define SCHED_FIFO 1
#define SCHED_RR 2

struct sched_param
{
  int sched_priority; /* Process execution scheduling priority */
};

extern int sched_yield(void);

__END_DECLS

#endif