/*
 * $Id: time_tzset.c,v 1.1 2024-05-02 12:04:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _TIMEZONE_HEADERS_H
#include "timezone_headers.h"
#endif /* _TIMEZONE_HEADERS_H */

static char __tzname_std[TZNAME_MAX + 2];
static char __tzname_dst[TZNAME_MAX + 2];
static char *prev_tzenv = NULL;

const uint8_t __month_lengths[2][MONSPERYEAR] = {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

/* Shared timezone information for libc/time functions.  */
static __tzinfo_type tzinfo = {1, 0,
   { {'J', 0, 0, 0, 0, (time_t)0, 0L },
     {'J', 0, 0, 0, 0, (time_t)0, 0L }
   }
};

__tzinfo_type *__gettzinfo (void) {
    return &tzinfo;
}

int
__tzcalc_limits(int year) {
    int days, year_days, years;
    int i, j;
    __tzinfo_type *const tz = __gettzinfo();

    if (year < EPOCH_YEAR)
        return 0;

    tz->__tzyear = year;

    years = (year - EPOCH_YEAR);

    year_days = years * 365 +
                (years - 1 + EPOCH_YEARS_SINCE_LEAP) / 4 -
                (years - 1 + EPOCH_YEARS_SINCE_CENTURY) / 100 +
                (years - 1 + EPOCH_YEARS_SINCE_LEAP_CENTURY) / 400;

    for (i = 0; i < 2; ++i) {
        if (tz->__tzrule[i].ch == 'J') {
            /* The Julian day n (1 <= n <= 365). */
            days = year_days + tz->__tzrule[i].d +
                   (isleap(year) && tz->__tzrule[i].d >= 60);
            /* Convert to yday */
            --days;
        } else if (tz->__tzrule[i].ch == 'D')
            days = year_days + tz->__tzrule[i].d;
        else {
            const int yleap = isleap(year);
            int m_day, m_wday, wday_diff;
            const uint8_t *const ip = __month_lengths[yleap];

            days = year_days;

            for (j = 1; j < tz->__tzrule[i].m; ++j)
                days += ip[j - 1];

            m_wday = (EPOCH_WDAY + days) % DAYSPERWEEK;

            wday_diff = tz->__tzrule[i].d - m_wday;
            if (wday_diff < 0)
                wday_diff += DAYSPERWEEK;
            m_day = (tz->__tzrule[i].n - 1) * DAYSPERWEEK + wday_diff;

            while (m_day >= ip[j - 1])
                m_day -= DAYSPERWEEK;

            days += m_day;
        }

        /* store the change-over time in GMT form by adding offset */
        tz->__tzrule[i].change = (time_t) days * SECSPERDAY +
                                 tz->__tzrule[i].s + tz->__tzrule[i].offset;
    }

    tz->__tznorth = (tz->__tzrule[0].change < tz->__tzrule[1].change);

    return 1;
}

void
_tzset_unlocked(void) {
    char *tzenv;
    unsigned short hh, mm, ss, m, w, d;
    int sign, n;
    int i, ch;
    long offset0, offset1;
    __tzinfo_type *tz = __gettzinfo();
    static const struct __tzrule_struct default_tzrule = {'J', 0, 0, 0, 0, (time_t) 0, 0L};
    struct _clib4 *__clib4 = __CLIB4;

    if ((tzenv = getenv("TZ")) == NULL) {
        if (__clib4->__dyntz == TRUE) {
            FreeVec(__clib4->__tzname[0]);
            FreeVec(__clib4->__tzname[1]);
            __clib4->__dyntz = FALSE;
        }
        __clib4->__timezone = 0;
        __clib4->__daylight = 0;
        __clib4->__tzname[0] = "GMT";
        __clib4->__tzname[1] = "GMT";
        tz->__tzrule[0] = default_tzrule;
        tz->__tzrule[1] = default_tzrule;
        if (prev_tzenv != NULL) {
            FreeVec(prev_tzenv);
            prev_tzenv = NULL;
        }
        return;
    }

    if (prev_tzenv != NULL && strcmp(tzenv, prev_tzenv) == 0)
        return;

    if (prev_tzenv)
        FreeVec(prev_tzenv);
    prev_tzenv = AllocVecTags(strlen(tzenv) + 1, AVT_Type, MEMF_SHARED, TAG_DONE);

    if (prev_tzenv != NULL)
        strcpy(prev_tzenv, tzenv);

    if (__clib4->__dyntz == TRUE) {
        FreeVec(__clib4->__tzname[0]);
        FreeVec(__clib4->__tzname[1]);
        __clib4->__dyntz = FALSE;
    }

    /* default to unnamed UTC in case of error */
    __clib4->__timezone = 0;
    __clib4->__daylight = 0;
    __clib4->__tzname[0] = "";
    __clib4->__tzname[1] = "";
    tz->__tzrule[0] = default_tzrule;
    tz->__tzrule[1] = default_tzrule;

    /* ignore implementation-specific format specifier */
    if (*tzenv == ':')
        ++tzenv;

    /* allow POSIX angle bracket < > quoted signed alphanumeric tz abbr e.g. <MESZ+0330> */
    if (*tzenv == '<') {
        ++tzenv;

        /* quit if no items, too few or too many chars, or no close quote '>' */
        if (sscanf(tzenv, "%11[-+0-9A-Za-z]%n", __tzname_std, &n) <= 0
            || n < TZNAME_MIN || TZNAME_MAX < n || '>' != tzenv[n])
            return;

        ++tzenv;    /* bump for close quote '>' */
    } else {
        /* allow POSIX unquoted alphabetic tz abbr e.g. MESZ */
        if (sscanf(tzenv, "%11[A-Za-z]%n", __tzname_std, &n) <= 0
            || n < TZNAME_MIN || TZNAME_MAX < n)
            return;
    }

    tzenv += n;

    sign = 1;
    if (*tzenv == '-') {
        sign = -1;
        ++tzenv;
    } else if (*tzenv == '+')
        ++tzenv;

    mm = 0;
    ss = 0;

    if (sscanf(tzenv, "%hu%n:%hu%n:%hu%n", &hh, &n, &mm, &n, &ss, &n) < 1)
        return;

    offset0 = sign * (ss + SECSPERMIN * mm + SECSPERHOUR * hh);
    tzenv += n;

    /* allow POSIX angle bracket < > quoted signed alphanumeric tz abbr e.g. <MESZ+0330> */
    if (*tzenv == '<') {
        ++tzenv;

        /* quit if no items, too few or too many chars, or no close quote '>' */
        if (sscanf(tzenv, "%11[-+0-9A-Za-z]%n", __tzname_dst, &n) <= 0 && tzenv[0] == '>') { /* No dst */
            if (__clib4->__dyntz == TRUE) {
                FreeVec(__clib4->__tzname[0]);
                FreeVec(__clib4->__tzname[1]);
                __clib4->__dyntz = FALSE;
            }

            __clib4->__tzname[0] = __tzname_std;
            __clib4->__tzname[1] = __clib4->__tzname[0];
            tz->__tzrule[0].offset = offset0;
            __clib4->__timezone = offset0;
            return;
        } else if (n < TZNAME_MIN || TZNAME_MAX < n || '>' != tzenv[n]) { /* error */
            return;
        }

        ++tzenv;    /* bump for close quote '>' */
    } else {
        /* allow POSIX unquoted alphabetic tz abbr e.g. MESZ */
        if (sscanf(tzenv, "%11[A-Za-z]%n", __tzname_dst, &n) <= 0) { /* No dst */
            if (__clib4->__dyntz == TRUE) {
                FreeVec(__clib4->__tzname[0]);
                FreeVec(__clib4->__tzname[1]);
                __clib4->__dyntz = FALSE;
            }

            __clib4->__tzname[0] = __tzname_std;
            __clib4->__tzname[1] = __clib4->__tzname[0];
            tz->__tzrule[0].offset = offset0;
            __clib4->__timezone = offset0;
            return;
        } else if (n < TZNAME_MIN || TZNAME_MAX < n) { /* error */
            return;
        }
    }

    tzenv += n;

    /* otherwise we have a dst name, look for the offset */
    sign = 1;
    if (*tzenv == '-') {
        sign = -1;
        ++tzenv;
    } else if (*tzenv == '+')
        ++tzenv;

    hh = 0;
    mm = 0;
    ss = 0;

    n = 0;
    if (sscanf(tzenv, "%hu%n:%hu%n:%hu%n", &hh, &n, &mm, &n, &ss, &n) <= 0)
        offset1 = offset0 - 3600;
    else
        offset1 = sign * (ss + SECSPERMIN * mm + SECSPERHOUR * hh);

    tzenv += n;

    for (i = 0; i < 2; ++i) {
        if (*tzenv == ',')
            ++tzenv;

        if (*tzenv == 'M') {
            if (sscanf(tzenv, "M%hu%n.%hu%n.%hu%n", &m, &n, &w, &n, &d, &n) != 3 ||
                m < 1 || m > 12 || w < 1 || w > 5 || d > 6)
                return;

            tz->__tzrule[i].ch = 'M';
            tz->__tzrule[i].m = m;
            tz->__tzrule[i].n = w;
            tz->__tzrule[i].d = d;

            tzenv += n;
        } else {
            char *end;
            if (*tzenv == 'J') {
                ch = 'J';
                ++tzenv;
            } else
                ch = 'D';

            d = strtoul(tzenv, &end, 10);

            /* if unspecified, default to US settings */
            /* From 1987-2006, US was M4.1.0,M10.5.0, but starting in 2007 is
             * M3.2.0,M11.1.0 (2nd Sunday March through 1st Sunday November)  */
            if (end == tzenv) {
                if (i == 0) {
                    tz->__tzrule[0].ch = 'M';
                    tz->__tzrule[0].m = 3;
                    tz->__tzrule[0].n = 2;
                    tz->__tzrule[0].d = 0;
                } else {
                    tz->__tzrule[1].ch = 'M';
                    tz->__tzrule[1].m = 11;
                    tz->__tzrule[1].n = 1;
                    tz->__tzrule[1].d = 0;
                }
            } else {
                tz->__tzrule[i].ch = ch;
                tz->__tzrule[i].d = d;
            }

            tzenv = end;
        }

        /* default time is 02:00:00 am */
        hh = 2;
        mm = 0;
        ss = 0;
        n = 0;

        if (*tzenv == '/')
            if (sscanf(tzenv, "/%hu%n:%hu%n:%hu%n", &hh, &n, &mm, &n, &ss, &n) <= 0) {
                /* error in time format, restore tz rules to default and return */
                tz->__tzrule[0] = default_tzrule;
                tz->__tzrule[1] = default_tzrule;
                return;
            }

        tz->__tzrule[i].s = ss + SECSPERMIN * mm + SECSPERHOUR * hh;

        tzenv += n;
    }
    
    if (__clib4->__dyntz == TRUE) {
        FreeVec(__clib4->__tzname[0]);
        FreeVec(__clib4->__tzname[1]);
        __clib4->__dyntz = FALSE;
    }

    tz->__tzrule[0].offset = offset0;
    tz->__tzrule[1].offset = offset1;
    __clib4->__tzname[0] = __tzname_std;
    __clib4->__tzname[1] = __tzname_dst;
    __tzcalc_limits(tz->__tzyear);
    __clib4->__timezone = tz->__tzrule[0].offset;
    __clib4->__daylight = tz->__tzrule[0].offset != tz->__tzrule[1].offset;
}

void
tzset(void) {
    __timezone_lock();

    _tzset_unlocked();

    __timezone_unlock();
}

int __get_timezone(void) {
    return __CLIB4->__timezone;
}

int __get_daylight(void) {
    return __CLIB4->__daylight;
}

char **__get_tzname(void) {
    return __CLIB4->__tzname;
}