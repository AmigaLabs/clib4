#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H

#include <features.h>

__BEGIN_DECLS

#include <sys/types.h>

/* Structure describing CPU time used by a process and its children.  */
struct tms
{
    clock_t tms_utime;  /* User CPU time.  */
    clock_t tms_stime;  /* System CPU time.  */

    clock_t tms_cutime; /* User CPU time of dead children.  */
    clock_t tms_cstime; /* System CPU time of dead children.  */
};

/* Store the CPU time used by this process and all its
   dead children (and their dead children) in BUFFER.
   Return the elapsed real time, or (clock_t) -1 for errors.
   All times are in CLK_TCKths of a second.  */
extern clock_t times(struct tms *tp);

__END_DECLS

#endif /* _SYS_TIMES_H */