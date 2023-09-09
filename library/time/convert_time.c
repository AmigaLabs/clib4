/*
 * $Id: time_convert_time.c,v 2.0 2023-05-26 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

/* 2000-03-01 (mod 400 year, immediately after feb29 */
#define LEAPOCH (946684800LL + 86400*(31+29))

#define DAYS_PER_400Y (365*400 + 97)
#define DAYS_PER_100Y (365*100 + 24)
#define DAYS_PER_4Y   (365*4   + 1)

int __month_to_secs(int month, int is_leap) {
    static const int secs_through_month[] = {
            0, 31 * 86400, 59 * 86400, 90 * 86400,
            120 * 86400, 151 * 86400, 181 * 86400, 212 * 86400,
            243 * 86400, 273 * 86400, 304 * 86400, 334 * 86400};
    int t = secs_through_month[month];
    if (is_leap && month >= 2) t += 86400;
    return t;
}

long long __year_to_secs(long long year, int *is_leap) {
    if (year - 2ULL <= 136) {
        int y = year;
        int leaps = (y - 68) >> 2;
        if (!((y - 68) & 3)) {
            leaps--;
            if (is_leap) *is_leap = 1;
        } else if (is_leap) *is_leap = 0;
        return 31536000 * (y - 70) + 86400 * leaps;
    }

    int cycles, centuries, leaps, rem;

    if (!is_leap) is_leap = &(int) {0};
    cycles = (year - 100) / 400;
    rem = (year - 100) % 400;
    if (rem < 0) {
        cycles--;
        rem += 400;
    }
    if (!rem) {
        *is_leap = 1;
        centuries = 0;
        leaps = 0;
    } else {
        if (rem >= 200) {
            if (rem >= 300) centuries = 3, rem -= 300;
            else centuries = 2, rem -= 200;
        } else {
            if (rem >= 100) centuries = 1, rem -= 100;
            else centuries = 0;
        }
        if (!rem) {
            *is_leap = 0;
            leaps = 0;
        } else {
            leaps = rem / 4U;
            rem %= 4U;
            *is_leap = !rem;
        }
    }

    leaps += 97 * cycles + 24 * centuries - *is_leap;

    return (year - 100) * 31536000LL + leaps * 86400LL + 946684800 + 86400;
}

long long __tm_to_secs(const struct tm *tm) {
    int is_leap;
    long long year = tm->tm_year;
    int month = tm->tm_mon;
    if (month >= 12 || month < 0) {
        int adj = month / 12;
        month %= 12;
        if (month < 0) {
            adj--;
            month += 12;
        }
        year += adj;
    }
    long long t = __year_to_secs(year, &is_leap);
    t += __month_to_secs(month, is_leap);
    t += 86400LL * (tm->tm_mday - 1);
    t += 3600LL * tm->tm_hour;
    t += 60LL * tm->tm_min;
    t += tm->tm_sec;
    return t;
}

int __secs_to_tm(long long t, struct tm *tm) {
    long long days, secs, years;
    int remdays, remsecs, remyears;
    int qc_cycles, c_cycles, q_cycles;
    int months;
    int wday, yday, leap;
    static const char days_in_month[] = {31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29};

    /* Reject time_t values whose year would overflow int */
    if (t < INT_MIN * 31622400LL || t > INT_MAX * 31622400LL)
        return -1;

    secs = t - LEAPOCH;
    days = secs / 86400;
    remsecs = secs % 86400;
    if (remsecs < 0) {
        remsecs += 86400;
        days--;
    }

    wday = (3 + days) % 7;
    if (wday < 0) wday += 7;

    qc_cycles = days / DAYS_PER_400Y;
    remdays = days % DAYS_PER_400Y;
    if (remdays < 0) {
        remdays += DAYS_PER_400Y;
        qc_cycles--;
    }

    c_cycles = remdays / DAYS_PER_100Y;
    if (c_cycles == 4) c_cycles--;
    remdays -= c_cycles * DAYS_PER_100Y;

    q_cycles = remdays / DAYS_PER_4Y;
    if (q_cycles == 25) q_cycles--;
    remdays -= q_cycles * DAYS_PER_4Y;

    remyears = remdays / 365;
    if (remyears == 4) remyears--;
    remdays -= remyears * 365;

    leap = !remyears && (q_cycles || !c_cycles);
    yday = remdays + 31 + 28 + leap;
    if (yday >= 365 + leap) yday -= 365 + leap;

    years = remyears + 4 * q_cycles + 100 * c_cycles + 400LL * qc_cycles;

    for (months = 0; days_in_month[months] <= remdays; months++)
        remdays -= days_in_month[months];

    if (months >= 10) {
        months -= 12;
        years++;
    }

    if (years + 100 > INT_MAX || years + 100 < INT_MIN)
        return -1;

    tm->tm_year = years + 100;
    tm->tm_mon = months + 2;
    tm->tm_mday = remdays + 1;
    tm->tm_wday = wday;
    tm->tm_yday = yday;

    tm->tm_hour = remsecs / 3600;
    tm->tm_min = remsecs / 60 % 60;
    tm->tm_sec = remsecs % 60;

    return 0;
}

BOOL
__convert_time_to_datestamp(time_t time_value, struct DateStamp *ds) {
    BOOL success;
    struct _clib2 *__clib2 = __CLIB2;

    /* Adjust the time to the AmigaOS epoch. */
    time_value -= UNIX_TIME_OFFSET;

    __locale_lock(__clib2);

    /* If possible, adjust the time to match the local time zone settings. */
    if (__clib2->__default_locale != NULL)
        time_value -= 60 * __clib2->__default_locale->loc_GMTOffset;

    __locale_unlock(__clib2);

    ds->ds_Days = (time_value / (24 * 60 * 60));
    ds->ds_Minute = (time_value % (24 * 60 * 60)) / 60;
    ds->ds_Tick = (time_value % 60) * TICKS_PER_SECOND;

    success = TRUE;

    return (success);
}
