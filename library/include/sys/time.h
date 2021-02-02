#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <features.h>

__BEGIN_DECLS

#ifdef __USE_GNU
/* Macros for converting between `struct timeval' and `struct timespec'.  */
# define TIMEVAL_TO_TIMESPEC(tv, ts) {                                   \
        (ts)->tv_sec = (tv)->tv_sec;                                    \
        (ts)->tv_nsec = (tv)->tv_usec * 1000;                           \
}
# define TIMESPEC_TO_TIMEVAL(tv, ts) {                                   \
        (tv)->tv_sec = (ts)->tv_sec;                                    \
        (tv)->tv_usec = (ts)->tv_nsec / 1000;                           \
}
#endif

/****************************************************************************/

/* This is a tough one. Some code takes a bad turn if the <exec/types.h>
   header file is pulled in, which rules out that the structure definitions
   in <devices/timer.h> are used. We are only interested in the timeval
   structure and try to get by with this definition. Sometimes it works,
   sometimes it doesn't. Not sure if there really is a good solution for
   this problem... */
#if !defined(__TIMEVAL_ALREADY_DEFINED) && !defined(__USE_CLIB2_TIMEVAL)

/****************************************************************************/

/* The V40 header files (OS 3.x) and below will always define the
   'struct timeval' in <devices/timer.h>. But the V50 header
   files and beyond will not define 'struct timeval' if it is
   included from here. We start by checking which header files
   are being used. */
#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

/****************************************************************************/

/* Which header file version is being used? */
#if INCLUDE_VERSION < 50

/****************************************************************************/

/* This will define the 'struct timeval' */
#ifndef DEVICES_TIMER_H
#include <devices/timer.h>
#endif /* DEVICES_TIMER_H */

/* Assume that the 'struct timeval' definition is now in place. */
#define __TIMEVAL_ALREADY_DEFINED

/****************************************************************************/

#endif /* INCLUDE_VERSION */

/****************************************************************************/

#endif /* !__TIMEVAL_ALREADY_DEFINED && !__USE_CLIB2_TIMEVAL */

/****************************************************************************/

/* If the timeval is still undefined or should be defined locally,
   define our own now... */
#if !defined(__TIMEVAL_ALREADY_DEFINED)

/****************************************************************************/

/* We will have to make our own... */
struct timeval
{
	unsigned long tv_secs;
	unsigned long tv_micro;
};

/* Make sure that the 'struct timeval' is not redefined, should
   <devices/timer.h> get included again. */
#define __TIMEVAL_ALREADY_DEFINED

/****************************************************************************/

#endif /* !__TIMEVAL_ALREADY_DEFINED */

/****************************************************************************/

#ifndef tv_sec
#define tv_sec tv_secs
#endif /* tv_sec */

#ifndef tv_usec
#define tv_usec tv_micro
#endif /* tv_usec */

/****************************************************************************/

struct timezone
{
	int tz_minuteswest;	/* of Greenwich */
	int tz_dsttime;		/* type of dst correction to apply */
};

/****************************************************************************/

int gettimeofday(struct timeval *tp, struct timezone *tzp);

#ifdef __USE_MISC
/* Convenience macros for operations on timevals.
   NOTE: `timercmp' does not work for >= or <=.  */
# define timerisset(tvp)        ((tvp)->tv_sec || (tvp)->tv_usec)
# define timerclear(tvp)        ((tvp)->tv_sec = (tvp)->tv_usec = 0)
# define timercmp(a, b, CMP)                                                  \
  (((a)->tv_sec == (b)->tv_sec) ?                                             \
   ((a)->tv_usec CMP (b)->tv_usec) :                                          \
   ((a)->tv_sec CMP (b)->tv_sec))
# define timeradd(a, b, result)                                               \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                          \
    if ((result)->tv_usec >= 1000000)                                         \
      {                                                                       \
        ++(result)->tv_sec;                                                   \
        (result)->tv_usec -= 1000000;                                         \
      }                                                                       \
  } while (0)
# define timersub(a, b, result)                                               \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                          \
    if ((result)->tv_usec < 0) {                                              \
      --(result)->tv_sec;                                                     \
      (result)->tv_usec += 1000000;                                           \
    }                                                                         \
  } while (0)
#endif  /* Misc.  */

__END_DECLS

#endif /* sys/time.h */