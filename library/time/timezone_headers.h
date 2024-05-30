/*
 * $Id: timezone_headers.h,v 1.0 2020-01-15 10:00:23 clib4devs Exp $
*/

#ifndef _TIMEZONE_HEADERS_H
#define _TIMEZONE_HEADERS_H

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif /* EXEC_LIBRARIES_H */

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif /* PROTO_EXEC_H */

#include <locale.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>


#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#ifndef _DEBUG_H
#include "debug.h"
#endif /* _DEBUG_H */

#include <time.h>

extern void __timezone_lock(void);
extern void __timezone_unlock(void);

typedef struct __tzrule_struct {
    char ch;
    int m; /* Month of year if ch=M */
    int n; /* Week of month if ch=M */
    int d; /* Day of week if ch=M, day of year if ch=J or ch=D */
    int s; /* Time of day in seconds */
    time_t change;
    long offset; /* Match type of _timezone. */
} __tzrule_type;

typedef struct __tzinfo_struct {
    int __tznorth;
    int __tzyear;
    __tzrule_type __tzrule[2];
} __tzinfo_type;

__tzinfo_type *__gettzinfo(void);
void _tzset_unlocked(void);
int __tzcalc_limits(int year);

#define SECSPERMIN	60L
#define MINSPERHOUR	60L
#define HOURSPERDAY	24L
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	(SECSPERHOUR * HOURSPERDAY)
#define DAYSPERWEEK	7
#define MONSPERYEAR	12

#define YEAR_BASE	1900
#define EPOCH_YEAR      1970
#define EPOCH_WDAY      4
#define EPOCH_YEARS_SINCE_LEAP 2
#define EPOCH_YEARS_SINCE_CENTURY 70
#define EPOCH_YEARS_SINCE_LEAP_CENTURY 370

extern const uint8_t __month_lengths[2][MONSPERYEAR];

static inline int isleap (int y) {
    // This routine must return exactly 0 or 1, because the result is used to index on __month_lengths[].
    // The order of checks below is the fastest for a random year.
    return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
}


#endif /* _TIMEZONE_HEADERS_H */
