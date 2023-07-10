/*
 * $Id: time.h,v 1.8 2023-06-03 12:06:14 clib2devs Exp $
*/

#ifndef _TIME_H
#define _TIME_H

#include <features.h>
#include <stddef.h>
#include <inttypes.h>
#include <sys/time.h>
#include <endian.h>
#include <sys/types.h>

__BEGIN_DECLS

/*
 * Divide the number returned by clock() by CLOCKS_PER_SEC to obtain
 * the elapsed time in seconds
 */
#define CLOCKS_PER_SEC ((clock_t)50)
#define CLK_TCK CLOCKS_PER_SEC

/* Identifier for system-wide realtime clock.  */
#define CLOCK_REALTIME      (clockid_t) 0
/* Monotonic system-wide clock.  */
#define CLOCK_MONOTONIC     (clockid_t) 1
/* Monotonic system-wide clock, not adjusted for frequency scaling.  */
#define CLOCK_MONOTONIC_RAW (clockid_t) 4

#define CLOCK_PROCESS_CPUTIME_ID (clockid_t)2 /* Not used in clock_ functions yet */
#define CLOCK_THREAD_CPUTIME_ID (clockid_t)3  /* Not used in clock_ functions yet */

#define NSEC_PER_SEC 1000000000ull
#define TIMER_ABSTIME 0x01

#if defined(_XOPEN_SOURCE) || defined(_BSD_SOURCE) || defined(_GNU_SOURCE)
#define __tm_gmtoff tm_gmtoff
#define __tm_zone tm_zone
#endif

#define _timezone (__CLIB2->__timezone)
#define _daylight (__CLIB2->__daylight)
#define _tzname   (__CLIB2->__tzname)

/* POSIX defines the external tzname being defined in time.h */
#ifndef tzname
#define tzname _tzname
#endif

struct tm {
    int tm_sec;   /* Number of seconds past the minute (0..59) */
    int tm_min;   /* Number of minutes past the hour (0..59) */
    int tm_hour;  /* Number of hours past the day (0..23) */
    int tm_mday;  /* Day of the month (1..31) */
    int tm_mon;   /* Month number (0..11) */
    int tm_year;  /* Year number minus 1900 */
    int tm_wday;  /* Day of the week (0..6; 0 is Sunday) */
    int tm_yday;  /* Day of the year (0..365) */
    int tm_isdst; /* Is this date using daylight savings time? */
    long __tm_gmtoff;
    const char *__tm_zone;
};

extern clock_t clock(void);
extern time_t time(time_t *t);
extern char *asctime(const struct tm *tm);
extern char *ctime(const time_t *t);
extern struct tm *gmtime(const time_t *t);
extern struct tm *localtime(const time_t *t);
extern time_t mktime(struct tm *tm);
extern double difftime(time_t t1, time_t t0);
extern size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *tm);
extern char *strptime(const char *buf, const char *fmt, struct tm *timeptr);

/* Timespec declaration */
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

struct timespec64 {
    time64_t tv_sec;  /* Seconds */
    int32_t : 32;     /* Padding */
    int32_t tv_nsec;  /* Nanoseconds */
};

/* BSD time macros used by RTEMS code */
/* Convenience macros for operations on timevals.
   NOTE: `timercmp' does not work for >= or <=.  */
#ifndef __USE_OLD_TIMEVAL__
#define timerisset(tvp) ((tvp)->tv_sec || (tvp)->tv_usec)
#define timerclear(tvp) ((tvp)->tv_sec = (tvp)->tv_usec = 0)
#define timercmp(a, b, CMP) \
  (((a)->tv_sec == (b)->tv_sec) ? ((a)->tv_usec CMP(b)->tv_usec) : ((a)->tv_sec CMP(b)->tv_sec))
#define timeradd(a, b, result)                       \
  do                                                 \
  {                                                  \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;    \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec; \
    if ((result)->tv_usec >= 1000000)                \
    {                                                \
      ++(result)->tv_sec;                            \
      (result)->tv_usec -= 1000000;                  \
    }                                                \
  } while (0)
#define timersub(a, b, result)                       \
  do                                                 \
  {                                                  \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;    \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
    if ((result)->tv_usec < 0)                       \
    {                                                \
      --(result)->tv_sec;                            \
      (result)->tv_usec += 1000000;                  \
    }                                                \
  } while (0)
#else
#define timerisset(tvp) ((tvp)->Seconds || (tvp)->Microseconds)
#define timerclear(tvp) ((tvp)->Seconds = (tvp)->Microseconds = 0)
#define timercmp(a, b, CMP) \
  (((a)->Seconds == (b)->Seconds) ? ((a)->Microseconds CMP(b)->Microseconds) : ((a)->Seconds CMP(b)->Seconds))
#define timeradd(a, b, result)                       \
  do                                                 \
  {                                                  \
    (result)->Seconds = (a)->Seconds + (b)->Seconds;    \
    (result)->Microseconds = (a)->Microseconds + (b)->Microseconds; \
    if ((result)->Microseconds >= 1000000)                \
    {                                                \
      ++(result)->Seconds;                            \
      (result)->Microseconds -= 1000000;                  \
    }                                                \
  } while (0)
#define timersub(a, b, result)                       \
  do                                                 \
  {                                                  \
    (result)->Seconds = (a)->Seconds - (b)->Seconds;    \
    (result)->Microseconds = (a)->Microseconds - (b)->Microseconds; \
    if ((result)->Microseconds < 0)                       \
    {                                                \
      --(result)->Seconds;                            \
      (result)->Microseconds += 1000000;                  \
    }                                                \
  } while (0)
#endif

/****************************************************************************/

extern char *asctime_r(const struct tm *tm, char *buffer);
extern char *ctime_r(const time_t *tptr, char *buffer);
extern struct tm *gmtime_r(const time_t *t, struct tm *tm_ptr);
extern struct tm *localtime_r(const time_t *t, struct tm *tm_ptr);
extern void tzset(void);
extern int nanosleep(const struct timespec *req, struct timespec *rem);
extern int clock_gettime(clockid_t clk_id, struct timespec *t);
extern int clock_settime(clockid_t clk_id, const struct timespec *t);
extern int clock_getres(clockid_t clock_id, struct timespec *res);
extern int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain);
extern unsigned long long rdtsc(void);
extern int clock_gettime64 (clockid_t clock_id, struct timespec64 *tp);

#if defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE)
extern int stime(const time_t *t);
extern time_t timegm(struct tm *tm);
#endif

/* Check whether T fits in time_t.  */
static inline int
in_time_t_range (time64_t t) {
    time_t s = t;
    return s == t;
}

/* Convert a known valid struct timeval into a struct timespec.  */
static inline struct timespec
valid_timeval_to_timespec(const struct timeval tv) {
    struct timespec ts;

    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;

    return ts;
}

/* Convert a known valid struct timeval into a struct timespec64.  */
static inline struct timespec64
valid_timeval_to_timespec64 (const struct timeval tv) {
    struct timespec64 ts64;

    ts64.tv_sec = tv.tv_sec;
    ts64.tv_nsec = tv.tv_usec * 1000;

    return ts64;
}

/* Convert a known valid struct timeval into a struct timeval64.  */
static inline struct timeval64
valid_timeval_to_timeval64(const struct timeval tv) {
    struct timeval64 tv64;

    tv64.tv_sec = tv.tv_sec;
    tv64.tv_usec = tv.tv_usec;

    return tv64;
}

/* Convert a valid and within range of struct timeval, struct timeval64 into a struct timeval.  */
static inline struct timeval
valid_timeval64_to_timeval (const struct timeval64 tv64) {
    struct timeval tv;

    tv.tv_sec = (long) tv64.tv_sec;
    tv.tv_usec = (long) tv64.tv_usec;

    return tv;
}

/* Convert a struct timeval64 into a struct timespec64.  */
static inline struct timespec64
timeval64_to_timespec64 (const struct timeval64 tv64) {
    struct timespec64 ts64;

    ts64.tv_sec = tv64.tv_sec;
    ts64.tv_nsec = tv64.tv_usec * 1000;

    return ts64;
}

/* Convert a known valid struct timespec into a struct timespec64.  */
static inline struct timespec64
valid_timespec_to_timespec64 (const struct timespec ts) {
    struct timespec64 ts64;

    ts64.tv_sec = ts.tv_sec;
    ts64.tv_nsec = ts.tv_nsec;

    return ts64;
}

/* Convert a valid and within range of struct timespec, struct timespec64 into a struct timespec.  */
static inline struct timespec
valid_timespec64_to_timespec (const struct timespec64 ts64) {
    struct timespec ts;

    ts.tv_sec = (time_t) ts64.tv_sec;
    ts.tv_nsec = ts64.tv_nsec;

    return ts;
}

/* Convert a valid and within range of struct timeval struct timespec64 into a struct timeval.  */
static inline struct timeval
valid_timespec64_to_timeval (const struct timespec64 ts64) {
    struct timeval tv;

    tv.tv_sec = (time_t) ts64.tv_sec;
    tv.tv_usec = ts64.tv_nsec / 1000;

    return tv;
}

/* Convert a valid and within range of struct timeval struct timespec into a struct timeval.  */
static inline struct timeval
valid_timespec_to_timeval (const struct timespec ts) {
    struct timeval tv;

    tv.tv_sec = (time_t) ts.tv_sec;
    tv.tv_usec = ts.tv_nsec / 1000;

    return tv;
}

/* Convert a struct timespec64 into a struct timeval64.  */
static inline struct timeval64
timespec64_to_timeval64 (const struct timespec64 ts64) {
    struct timeval64 tv64;

    tv64.tv_sec = ts64.tv_sec;
    tv64.tv_usec = ts64.tv_nsec / 1000;

    return tv64;
}

__END_DECLS

#endif /* _TIME_H */
