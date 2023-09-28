/*
 * $Id: time_asctime_r.c,v 1.10 2006-09-25 13:43:06 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

STATIC VOID
add_to_string(char *to, size_t to_size, const char *string, size_t *offset_ptr) {
    size_t offset, len;

    assert(to != NULL && to_size > 0 && string != NULL && offset_ptr != NULL);

    offset = (*offset_ptr);

    assert(offset < to_size);

    len = strlen(string);
    if (offset + len > to_size)
        len = to_size - offset;

    if (len > 0) {
        memmove(&to[offset], string, len);

        offset += len;

        (*offset_ptr) = offset;
    }
}

/****************************************************************************/

char *
__asctime_r(const struct tm *tm, char *buffer, size_t buffer_size) {
    char *result = NULL;
    size_t offset = 0;
    struct tm copy_tm;
    char number[16];
    const char *b;

    ENTER();

    SHOWPOINTER(tm);

    assert(tm != NULL || buffer == NULL);

    if (tm == NULL || buffer == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    if (buffer_size == 0)
        goto out;

    buffer_size--;

    /* Fill in the week day if it's not in proper range. */
    if (tm->tm_wday < 0 || tm->tm_wday > 6) {
        /* We use a peculiar algorithm rather than falling back onto
           mktime() here in order to avoid trouble with skewed results
           owing to time zone influence. */
        copy_tm = (*tm);
        copy_tm.tm_wday = __calculate_weekday(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);

        tm = &copy_tm;
    }

    assert(0 <= tm->tm_wday && tm->tm_wday <= 6);

    b = __abbreviated_week_day_names[tm->tm_wday];

    add_to_string(buffer, buffer_size, b, &offset);
    add_to_string(buffer, buffer_size, " ", &offset);

    if (0 <= tm->tm_mon && tm->tm_mon <= 11)
        b = __abbreviated_month_names[tm->tm_mon];
    else
        b = "---";

    add_to_string(buffer, buffer_size, b, &offset);
    add_to_string(buffer, buffer_size, " ", &offset);

    if (1 <= tm->tm_mday && tm->tm_mday <= 31)
        b = __number_to_string((unsigned int) tm->tm_mday, number, sizeof(number), 2);
    else
        b = "--";

    add_to_string(buffer, buffer_size, b, &offset);
    add_to_string(buffer, buffer_size, " ", &offset);

    if (0 <= tm->tm_hour && tm->tm_hour <= 23)
        b = __number_to_string((unsigned int) tm->tm_hour, number, sizeof(number), 2);
    else
        b = "--";

    add_to_string(buffer, buffer_size, b, &offset);
    add_to_string(buffer, buffer_size, ":", &offset);

    if (0 <= tm->tm_min && tm->tm_min <= 59)
        b = __number_to_string((unsigned int) tm->tm_min, number, sizeof(number), 2);
    else
        b = "--";

    add_to_string(buffer, buffer_size, b, &offset);
    add_to_string(buffer, buffer_size, ":", &offset);

    if (0 <= tm->tm_sec && tm->tm_sec <= 59)
        b = __number_to_string((unsigned int) tm->tm_sec, number, sizeof(number), 2);
    else
        b = "--";

    add_to_string(buffer, buffer_size, b, &offset);
    add_to_string(buffer, buffer_size, " ", &offset);

    if (0 <= tm->tm_year)
        b = __number_to_string((unsigned int) 1900 + tm->tm_year, number, sizeof(number), 0);
    else
        b = "----";

    add_to_string(buffer, buffer_size, b, &offset);

    SHOWSTRING(buffer);

    add_to_string(buffer, buffer_size, "\n", &offset);

    assert(offset <= buffer_size);
    buffer[offset] = '\0';

    result = buffer;

out:

    RETURN(result);
    return (result);
}

/****************************************************************************/

char *
asctime_r(const struct tm *tm, char *buffer) {
    char *result;

    ENTER();

    result = __asctime_r(tm, buffer, 40);

    RETURN(result);
    return (result);
}
