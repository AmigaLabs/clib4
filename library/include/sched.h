/*
 * $Id: sched.h,v 1.2 2021-01-14 12:06:14 clib2devs Exp $
*/

#ifndef SCHED_H
#define SCHED_H

#include <features.h>

__BEGIN_DECLS

#define SCHED_OTHER 0
#define SCHED_FIFO 1
#define SCHED_RR 2

struct sched_param {
    int sched_priority; /* Process execution scheduling priority */
};

extern int sched_get_priority_max(int);
extern int sched_get_priority_min(int);
extern int sched_getparam(pid_t, struct sched_param *);
extern int sched_getscheduler(pid_t);
extern int sched_rr_get_interval(pid_t, struct timespec *);
extern int sched_setparam(pid_t, const struct sched_param *);
extern int sched_setscheduler(pid_t, int, const struct sched_param *);
extern int sched_yield(void);

__END_DECLS

#endif