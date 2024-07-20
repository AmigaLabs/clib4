/*
 * $Id: time_strftime.c,v 1.19 2006-04-05 06:43:56 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

struct format_hook_data {
    char *buffer;
    int max_size;
    int len;
};

static void
format_hook_function(
        struct Hook *hook,
        struct Locale *unused_locale,
        ULONG c) {
    struct format_hook_data *data = hook->h_Data;
    (void) (unused_locale);

    if (c != '\0') {
        if (data->max_size > 0) {
            /* There's still room for another character. */
            (*data->buffer++) = (char) c;

            data->len++;

            data->max_size--;
        } else {
            /* Indicate that the buffer was too short. */
            data->len = 0;
        }
    }
}

STATIC VOID
store_string_via_hook(const char *string, int len, struct Hook *hook) {
    DECLARE_UTILITYBASE();

    assert(string != NULL && hook != NULL);
    assert(UtilityBase != NULL);

    if (len < 0)
        len = strlen(string);

    while (len-- > 0)
        CallHookPkt(hook, NULL, (APTR)((ULONG)(*string++)));
}

/* The algorithm for calculating the ISO 8601 week number value comes from
   the "Calendar FAQ" at <http://www.tondering.dk/claus/calendar.html>. */
inline static int
julian_day(int day, int month, int year) {
    int a, y, m, result;

    a = (14 - month) / 12;
    y = year + 4800 - a;
    m = month + 12 * a - 3;

    result = day + (153 * m + 2) / 5 + 365 * y + (y / 4) - (y / 100) + (y / 400) - 32045;

    return (result);
}

static void
iso8601_calendar_week_and_year(int day, int month, int year, int *week_ptr, int *year_ptr) {
    int J = julian_day(day, month, year);
    int d1, d4, L;
    int week;

    d4 = (J + 31741 - (J % 7)) % 146097 % 36524 % 1461;
    L = d4 / 1460;
    d1 = ((d4 - L) % 365) + L;

    week = (d1 / 7) + 1;

    if (month == 1 && week > 51)
        year--;
    else if (month == 12 && week == 1)
        year++;

    if (week_ptr != NULL)
        (*week_ptr) = week;

    if (year_ptr != NULL)
        (*year_ptr) = year;
}

static void
format_date(const char *format, const struct tm *tm, struct Hook *hook) {
    int gmt_offset;
    int week_number;
    int year_number;
    int hour;
    char buffer[40] = {0};
    const char *str;
    char c;
    struct _clib4 *__clib4 = __CLIB4;

    assert(format != NULL && tm != NULL && hook != NULL);

    while ((c = (*format++)) != '\0') {
        /* This is the simple case. */
        if (c != '%') {
            store_string_via_hook(&c, 1, hook);
            continue;
        }

        /* OK, what kind of format is expected? */
        c = (*format++);

        /* For C99, check if the 'E' or 'O' modifiers are present. If so,
           skip them for now. */
        if (c == 'E' || c == 'O')
            c = (*format++);

        /* We stop if the string ends here. Hm... should this count as an error? */
        if (c == '\0')
            break;

        switch (c) {
            /* Abbreviated weekday name ("Sun"). */
            case 'a':

                assert(0 <= tm->tm_wday && tm->tm_wday <= 6);

                if (0 <= tm->tm_wday && tm->tm_wday <= 6)
                    str = __abbreviated_week_day_names[tm->tm_wday];
                else
                    str = "-";

                store_string_via_hook(str, -1, hook);
                break;

                /* Full weekday name ("Sunday"). */
            case 'A':

                assert(0 <= tm->tm_wday && tm->tm_wday <= 6);

                if (0 <= tm->tm_wday && tm->tm_wday <= 6)
                    str = __week_day_names[tm->tm_wday];
                else
                    str = "-";

                store_string_via_hook(str, -1, hook);
                break;

                /* Abbreviated month name ("Jan"). */
            case 'b':

                assert(0 <= tm->tm_mon && tm->tm_mon <= 11);

                if (0 <= tm->tm_mon && tm->tm_mon <= 11)
                    str = __abbreviated_month_names[tm->tm_mon];
                else
                    str = "-";

                store_string_via_hook(str, -1, hook);
                break;

                /* Full month name ("January"). */
            case 'B':

                assert(0 <= tm->tm_mon && tm->tm_mon <= 11);

                if (0 <= tm->tm_mon && tm->tm_mon <= 11)
                    str = __month_names[tm->tm_mon];
                else
                    str = "";

                store_string_via_hook(str, -1, hook);
                break;

                /* Locale specific date and time ("%a %b %e %T %Y"). */
            case 'c':

                format_date("%a %b %e %T %Y", tm, hook);
                break;

                /* The century number ("00"-"99"; C99). */
            case 'C':

                __number_to_string((unsigned int) (tm->tm_year / 100), buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* Day of the month ("01"-"31"). */
            case 'd':

                assert(1 <= tm->tm_mday && tm->tm_mday <= 31);

                __number_to_string((unsigned int) tm->tm_mday, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* Date ("12/31/00"; C99) */
            case 'D':

                format_date("%m/%d/%y", tm, hook);
                break;

                /* Month with a leading space rather than a zero (" 1"-"12"; C99). */
            case 'e':

                assert(0 <= tm->tm_mon && tm->tm_mon <= 11);

                __number_to_string((unsigned int) tm->tm_mon + 1, buffer, sizeof(buffer), 2);
                if (buffer[0] == '0')
                    buffer[0] = ' ';

                store_string_via_hook(buffer, 2, hook);
                break;

                /* ISO 8601 date format ("2005-05-14"; C99). */
            case 'F':

                format_date("%Y-%m-%d", tm, hook);
                break;

                /* The last two digits of the week-based year ("00"-"99"; C99). */
            case 'g':

                assert(1 <= tm->tm_mday && tm->tm_mday <= 31);
                assert(0 <= tm->tm_mon && tm->tm_mon <= 11);
                assert(0 <= tm->tm_year);

                iso8601_calendar_week_and_year(tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, NULL, &year_number);
                __number_to_string(year_number % 100, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* The number of the week-based year ("2005"; C99). */
            case 'G':

                assert(1 <= tm->tm_mday && tm->tm_mday <= 31);
                assert(0 <= tm->tm_mon && tm->tm_mon <= 11);
                assert(0 <= tm->tm_year);

                iso8601_calendar_week_and_year(tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, NULL, &year_number);
                __number_to_string(year_number, buffer, sizeof(buffer), 0);
                store_string_via_hook(buffer, -1, hook);
                break;

                /* Abbreviated month name ("Jan"; C99). */
            case 'h':

                format_date("%b", tm, hook);
                break;

                /* Hour ("00"-"23"). */
            case 'H':

                assert(0 <= tm->tm_hour && tm->tm_hour <= 23);

                __number_to_string((unsigned int) tm->tm_hour, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* Hour ("01"-"12"). */
            case 'I':

                assert(0 <= tm->tm_hour && tm->tm_hour <= 23);

                hour = tm->tm_hour % 12;
                if (hour == 0)
                    hour = 12;

                __number_to_string(hour, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* Day of the year ("001"-"366"). */
            case 'j':

                __number_to_string(1 + __calculate_days_per_date(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday) -
                                   __calculate_days_per_date(tm->tm_year + 1900, 1, 1), buffer, sizeof(buffer), 3);
                store_string_via_hook(buffer, -1, hook);
                break;

                /* Month ("01"-"12"). */
            case 'm':

                assert(0 <= tm->tm_mon && tm->tm_mon <= 11);

                __number_to_string((unsigned int) tm->tm_mon + 1, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* Minute ("00"-"59"). */
            case 'M':

                assert(0 <= tm->tm_min && tm->tm_min <= 59);

                __number_to_string((unsigned int) tm->tm_min, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* Line feed character (C99). */
            case 'n':

                store_string_via_hook("\n", 1, hook);
                break;

                /* 'Ante meridiem'/'Post meridiem' indicator. */
            case 'p':

                assert(0 <= tm->tm_hour && tm->tm_hour <= 23);

                store_string_via_hook((tm->tm_hour < 12) ? "AM" : "PM", 2, hook);
                break;

                /* 12 hour clock time (C99). */
            case 'r':

                format_date("%I:%M:%S %p", tm, hook);
                break;

                /* Locale-specific time (C99). */
            case 'R':

                format_date("%H:%M", tm, hook);
                break;

                /* Seconds ("00"-"59"). */
            case 'S':

                assert(0 <= tm->tm_sec && tm->tm_sec <= 59);

                __number_to_string((unsigned int) tm->tm_sec, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* Horizontal tabulator character (C99). */
            case 't':

                store_string_via_hook("\t", 1, hook);
                break;

                /* ISO 8601 time format ("23:59:59"; C99). */
            case 'T':

                format_date("%H:%M:%S", tm, hook);
                break;

                /* ISO 8601 week day number ("1"-"7"; 1 is Monday; C99). */
            case 'u':

                assert(0 <= tm->tm_wday && tm->tm_wday <= 6);

                __number_to_string((unsigned int) (tm->tm_wday > 0 ? tm->tm_wday : 7), buffer, sizeof(buffer), 0);
                store_string_via_hook(buffer, 1, hook);
                break;

                /* Week number of the year; first week is the one that contains
                     * the first Sunday of the year ("00"-"53").
                     */
            case 'U':

                __number_to_string((tm->tm_yday + 7 - tm->tm_wday) / 7, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* ISO 8601 week number ("01"-"53"; C99). */
            case 'V':

                assert(1 <= tm->tm_mday && tm->tm_mday <= 31);
                assert(0 <= tm->tm_mon && tm->tm_mon <= 11);
                assert(0 <= tm->tm_year);

                iso8601_calendar_week_and_year(tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, &week_number, NULL);
                __number_to_string(week_number, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* Week day ("0"-"6"). */
            case 'w':

                assert(0 <= tm->tm_wday && tm->tm_wday <= 6);

                __number_to_string((unsigned int) tm->tm_wday, buffer, sizeof(buffer), 0);
                store_string_via_hook(buffer, 1, hook);
                break;

                /* Week number of the year; first week is the one that contains
                     * the first Monday of the year ("00"-"53").
                     */
            case 'W':

                __number_to_string((tm->tm_yday + 7 - ((tm->tm_wday + 6) % 7)) / 7, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* Locale-specific date ("%m/%d/%y"). */
            case 'x':

                format_date("%m/%d/%y", tm, hook);
                break;

                /* Locale-specific time ("%T"). */
            case 'X':

                format_date("%T", tm, hook);
                break;

                /* Year without century ("00"-"99"). */
            case 'y':

                assert(0 <= tm->tm_year);

                __number_to_string((unsigned int) tm->tm_year % 100, buffer, sizeof(buffer), 2);
                store_string_via_hook(buffer, 2, hook);
                break;

                /* Year with century ("1970"-"2147483647"). */
            case 'Y':

                assert(0 <= tm->tm_year);

                __number_to_string((unsigned int) 1900 + tm->tm_year, buffer, sizeof(buffer), 0);
                store_string_via_hook(buffer, -1, hook);
                break;

                /* ISO 8601 offset of time zone from UTC (C99). */
            case 'z':

                __locale_lock(__clib4);

                if (__clib4->__default_locale != NULL) {
                    gmt_offset = __clib4->__default_locale->loc_GMTOffset;
                    if (gmt_offset < 0)
                        gmt_offset = (-gmt_offset);
                    else if (gmt_offset > 0)
                        store_string_via_hook("-", 1, hook);
                } else {
                    gmt_offset = 0;
                }

                __locale_unlock(__clib4);

                /* The GMT offset is given in minutes. We need to print
                       it as a decimal number. */
                gmt_offset = (100 * (gmt_offset / 60)) + (gmt_offset % 60);

                __number_to_string((unsigned int) gmt_offset, buffer, sizeof(buffer), 0);
                store_string_via_hook(buffer, -1, hook);

                break;

                /* Time zone name. */
            case 'Z':

                store_string_via_hook("GMT", 3, hook);

                __locale_lock(__clib4);

                if (__clib4->__default_locale != NULL) {
                    int hours_west_of_gmt;
                    char sign = '?';

                    hours_west_of_gmt = __clib4->__default_locale->loc_GMTOffset / 60;
                    if (hours_west_of_gmt < 0) {
                        sign = '+';

                        hours_west_of_gmt = (-hours_west_of_gmt);
                    } else if (hours_west_of_gmt > 0) {
                        sign = '-';
                    }

                    if (hours_west_of_gmt != 0) {
                        store_string_via_hook(&sign, 1, hook);
                        __number_to_string((unsigned int) hours_west_of_gmt, buffer, sizeof(buffer), 2);
                        store_string_via_hook(buffer, -1, hook);
                    }
                }

                __locale_unlock(__clib4);

                break;

                /* Store that character 'as is'. */
            default:

                store_string_via_hook(&c, 1, hook);
                break;
        }
    }
}

size_t
strftime(char *s, size_t maxsize, const char *format, const struct tm *tm) {
    DECLARE_LOCALEBASE();
    size_t result = 0;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(s);
    SHOWVALUE(maxsize);
    SHOWSTRING(format);
    SHOWPOINTER(tm);

    assert(s != NULL && format != NULL && tm != NULL);
    assert((int) maxsize >= 0);

    if (s == NULL || format == NULL || tm == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    if (maxsize > 0) {
        struct format_hook_data data;
        struct Hook hook;

        data.len = 0;
        data.buffer = s;
        data.max_size = maxsize - 1;

        memset(&hook, 0, sizeof(hook));
        hook.h_Entry = (HOOKFUNC) format_hook_function;
        hook.h_Data = &data;

        __locale_lock(__clib4);

        /* Try to use the locale.library date/time conversion function. */
        if (__clib4->__locale_table[LC_TIME] != NULL) {
            struct DateStamp ds;
            struct tm tm_copy;
            time_t time_value;

            tm_copy = (*tm);

            time_value = mktime(&tm_copy);
            if (time_value == (time_t) - 1) {
                SHOWMSG("invalid time");
                goto out;
            }

            /* Convert the number of seconds into a DateStamp, as to be
               submitted to the FormatDate() function below. */
            if (CANNOT __convert_time_to_datestamp(time_value, &ds)) {
                SHOWMSG("time conversion to datestamp failed");
                goto out;
            }

            assert(LocaleBase != NULL);

            FormatDate(__clib4->__locale_table[LC_TIME], (STRPTR) format, &ds, &hook);
        } else {
            struct tm copy_tm;

            /* Fill in the week day if it's not in proper range. */
            if (tm->tm_wday < 0 || tm->tm_wday > 6) {
                /* We use a peculiar algorithm rather than falling back onto
                   mktime() here in order to avoid trouble with skewed results
                   owing to time zone influence. */
                copy_tm = (*tm);
                copy_tm.tm_wday = __calculate_weekday(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);

                tm = &copy_tm;
            }

            format_date(format, tm, &hook);
        }

        __locale_unlock(__clib4);

        (*data.buffer) = '\0';

        SHOWSTRING(s);

        result = data.len;
    }

out:

    RETURN(result);
    return (result);
}
