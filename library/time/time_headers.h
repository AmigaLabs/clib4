/*
 * $Id: time_headers.h,v 1.12 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#define _TIME_HEADERS_H

/****************************************************************************/

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif /* EXEC_LIBRARIES_H */

#ifndef EXEC_DEVICES_H
#include <exec/devices.h>
#endif /* EXEC_DEVICES_H */

#ifndef EXEC_MEMORY_H
#include <exec/memory.h>
#endif /* EXEC_MEMORY_H */

#ifndef DOS_DOSEXTENS_H
#include <dos/dosextens.h>
#endif /* DOS_DOSEXTENS_H */

#ifndef DEVICES_TIMER_H
#include <devices/timer.h>
#endif /* DEVICES_TIMER_H */

#ifndef LIBRARIES_LOCALE_H
#include <libraries/locale.h>
#endif /* LIBRARIES_LOCALE_H */

#ifndef UTILITY_DATE_H
#include <utility/date.h>
#endif /* UTILITY_DATE_H */

/****************************************************************************/

#ifndef CLIB_ALIB_PROTOS_H
#include <clib/alib_protos.h>
#endif /* CLIB_ALIB_PROTOS_H */

/****************************************************************************/

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif /* PROTO_EXEC_H */

#ifndef PROTO_DOS_H
#include <proto/dos.h>
#endif /* PROTO_DOS_H */

#ifndef PROTO_TIMER_H
#include <proto/timer.h>
#endif

#ifndef PROTO_TIMEZONE_H
#include <proto/timezone.h>
#endif

/****************************************************************************/

#include <string.h>
#include <locale.h>
#include <errno.h>
#include <time.h>
#include <dos.h>
#include <sys/times.h>
/****************************************************************************/

#ifndef _STDLIB_LOCALEBASE_H
#include "stdlib_localebase.h"
#endif /* _STDLIB_LOCALEBASE_H */

#ifndef _STDLIB_UTILITYBASE_H
#include "stdlib_utilitybase.h"
#endif /* _STDLIB_UTILITYBASE_H */

/****************************************************************************/

#ifndef _MATH_FP_SUPPORT_H
#include "math_fp_support.h"
#endif /* _MATH_FP_SUPPORT_H */

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#ifndef _DEBUG_H
#include "debug.h"
#endif /* _DEBUG_H */

struct itimer {
    int which;
};

/****************************************************************************/

/* This is the difference (in seconds) between the Unix epoch (which began
   on January 1st, 1970) and the AmigaOS epoch (which began eight years
   later on January 1st 1978). */
#define UNIX_TIME_OFFSET 252460800

/****************************************************************************/

extern const char * const NOCOMMON __abbreviated_week_day_names[7];
extern const char * const NOCOMMON __week_day_names[7];
extern const char * const NOCOMMON __abbreviated_month_names[12];
extern const char * const NOCOMMON __month_names[12];

/****************************************************************************/

extern char * __asctime_r(const struct tm *tm,char * buffer,size_t buffer_size);
extern char * __number_to_string(unsigned int number,char * string,size_t max_len,size_t min_len);
extern struct tm * __convert_time(ULONG seconds, LONG gmt_offset, struct tm * tm);
extern time_t __convert_datestamp_to_time(const struct DateStamp * ds);
extern BOOL __convert_time_to_datestamp(time_t time_value,struct DateStamp * ds);
extern int __calculate_weekday(int year,int month,int day);
extern int __calculate_days_per_date(int year,int month,int day);
extern int __secs_to_tm(long long t, struct tm *tm);
extern long long __tm_to_secs(const struct tm *tm);
extern long long __year_to_secs(long long year, int *is_leap);
extern int __month_to_secs(int month, int is_leap);
extern int __setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);

static inline void
fix_datestamp(struct DateStamp *ds) {
    const LONG ticks_per_minute = 60 * TICKS_PER_SECOND;
    const LONG minutes_per_day = 24 * 60;

    assert(ds != NULL);

    while (ds->ds_Minute >= minutes_per_day || ds->ds_Tick >= ticks_per_minute) {
        if (ds->ds_Minute >= minutes_per_day) {
            ds->ds_Days++;

            ds->ds_Minute -= minutes_per_day;
        }

        if (ds->ds_Tick >= ticks_per_minute) {
            ds->ds_Minute++;

            ds->ds_Tick -= ticks_per_minute;
        }
    }
}

static inline struct DateStamp *
timeval_to_datestamp(struct DateStamp *ds, const struct timeval *tv) {
    assert(ds != NULL && tv != NULL);

    ds->ds_Days = (tv->tv_sec / (24 * 60 * 60));
    ds->ds_Minute = (tv->tv_sec % (24 * 60 * 60)) / 60;
    ds->ds_Tick = (tv->tv_sec % 60) * TICKS_PER_SECOND + (TICKS_PER_SECOND * tv->tv_usec) / 1000000;

    fix_datestamp(ds);

    return (ds);
}

static inline void
add_dates(struct DateStamp *to, const struct DateStamp *from) {
    assert(to != NULL && from != NULL);

    to->ds_Tick += from->ds_Tick;
    to->ds_Minute += from->ds_Minute;
    to->ds_Days += from->ds_Days;

    fix_datestamp(to);
}
/****************************************************************************/

#endif /* _TIME_HEADERS_H */
