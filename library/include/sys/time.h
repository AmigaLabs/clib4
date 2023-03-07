#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <features.h>
#include <inttypes.h>

__BEGIN_DECLS

typedef int64_t time64_t;
typedef int64_t suseconds64_t;

#if defined(__TIMEVAL_ALREADY_DEFINED)
#ifdef __USE_OLD_TIMEVAL__
#define tv_sec  tv_secs
#define tv_usec tv_micro
#endif /* __USE_OLD_TIMEVAL__ */
#else
struct timeval {
    long tv_sec;
    long tv_usec;
};
/* Make sure that the 'struct timeval' is not redefined, should <devices/timer.h> get included again. */
#define __TIMEVAL_ALREADY_DEFINED
#ifdef __USE_OLD_TIMEVAL__
#define tv_secs  tv_sec
#define tv_micro tv_usec
#endif /* __USE_OLD_TIMEVAL__ */
#endif /* !__TIMEVAL_ALREADY_DEFINED */

struct timeval64 {
    time64_t tv_sec;            /* Seconds */
    suseconds64_t tv_usec;    /* Microseconds */
};

struct timezone {
    int tz_minuteswest; /* of Greenwich */
    int tz_dsttime;     /* type of dst correction to apply */
};

struct itimerval {
    struct timeval it_interval;
    struct timeval it_value;
};

#define DST_NONE 0 /* not on dst */
#define DST_USA 1  /* USA style dst */
#define DST_AUST 2 /* Australian style dst */
#define DST_WET 3  /* Western European dst */
#define DST_MET 4  /* Middle European dst */
#define DST_EET 5  /* Eastern European dst */
#define DST_CAN 6  /* Canada */

#define ITIMER_REAL    0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF    2

extern int gettimeofday(struct timeval *tp, struct timezone *tzp);
extern int settimeofday(const struct timeval *, const struct timezone *);
extern int utimes(const char *path, const struct timeval *tvp);
extern int getitimer(int which, struct itimerval *curr_value);
extern int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);

/* Macros for converting between `struct timeval' and `struct timespec'.  */
#if defined(_GNU_SOURCE)
#define TIMEVAL_TO_TIMESPEC(tv, ts) ( \
    (ts)->tv_sec = (tv)->tv_sec, \
    (ts)->tv_nsec = (tv)->tv_usec * 1000, \
    (void)0 )
#define TIMESPEC_TO_TIMEVAL(tv, ts) ( \
    (tv)->tv_sec = (ts)->tv_sec, \
    (tv)->tv_usec = (ts)->tv_nsec / 1000, \
    (void)0 )
#endif

__END_DECLS

#endif