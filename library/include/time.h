/*
 * $Id: time.h,v 1.7 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _TIME_H
#define _TIME_H

#include <features.h>

#ifndef _STDDEF_H
#include <stddef.h>
#endif /* _STDDEF_H */

#include <inttypes.h>

__BEGIN_DECLS

/****************************************************************************/

/*
 * Divide the number returned by clock() by CLOCKS_PER_SEC to obtain
 * the elapsed time in seconds
 */
#define CLOCKS_PER_SEC ((clock_t)50)
#define CLK_TCK CLOCKS_PER_SEC

/****************************************************************************/

#define CLOCK_REALTIME (clockid_t)0
#define CLOCK_MONOTONIC (clockid_t)1
#define CLOCK_PROCESS_CPUTIME_ID (clockid_t)2 /* Not used in clock_ functions yet */
#define CLOCK_THREAD_CPUTIME_ID (clockid_t)3  /* Not used in clock_ functions yet */

#define _CLOCK_T_ unsigned long long /* clock() */
#define _TIME_T_ long                /* time() */
#define _CLOCKID_T_ unsigned long
#define _TIMER_T_ unsigned long
#define NSEC_PER_SEC 1000000000ull
#define TIMER_ABSTIME 0x01

typedef _CLOCKID_T_ clockid_t;

typedef unsigned long clock_t;
typedef long long time_t;

struct tm
{
  int tm_sec;   /* Number of seconds past the minute (0..59) */
  int tm_min;   /* Number of minutes past the hour (0..59) */
  int tm_hour;  /* Number of hours past the day (0..23) */
  int tm_mday;  /* Day of the month (1..31) */
  int tm_mon;   /* Month number (0..11) */
  int tm_year;  /* Year number minus 1900 */
  int tm_wday;  /* Day of the week (0..6; 0 is Sunday) */
  int tm_yday;  /* Day of the year (0..365) */
  int tm_isdst; /* Is this date using daylight savings time? */
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
struct timespec
{
  time_t tv_secs;
  long tv_nsec;
};

#ifndef tv_sec
#define tv_sec tv_secs
#endif /* tv_sec */

#ifndef tv_usec
#define tv_usec tv_micro
#endif /* tv_usec */

/* BSD time macros used by RTEMS code */
/* Convenience macros for operations on timevals.
   NOTE: `timercmp' does not work for >= or <=.  */
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

extern uint64_t rdtsc(void);

/* Defined in localtime.c.  */
extern char *tzname[2];   /* Current timezone names.  */
extern int daylight;      /* If daylight-saving time is ever in use.  */
extern long int timezone; /* Seconds west of UTC.  */

__END_DECLS

#endif /* _TIME_H */
