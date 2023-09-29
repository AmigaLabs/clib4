/*
 * $Id: wchar_wcstod.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

double
wcstod(const wchar_t *nptr, wchar_t **endptr) {
    static const mbstate_t initial;
    mbstate_t mbs;
    double val;
    char *buf, *end;
    const wchar_t *wcp;
    size_t len;

    ENTER();

    while (iswspace(*nptr))
        nptr++;

    /*
     * Convert the supplied numeric wide char. string to multibyte.
     *
     * We could attempt to find the end of the numeric portion of the
     * wide char. string to avoid converting unneeded characters but
     * choose not to bother; optimising the uncommon case where
     * the input string contains a lot of text after the number
     * duplicates a lot of strtod()'s functionality and slows down the
     * most common cases.
     */
    wcp = nptr;
    mbs = initial;
    if ((len = wcsnrtombs(NULL, &wcp, (size_t) - 1, 0, &mbs)) == (size_t) - 1) {
        if (endptr != NULL)
            *endptr = (wchar_t *) nptr;
        RETURN(0.0);
        return (0.0);
    }
    if ((buf = malloc(len + 1)) == NULL) {
        RETURN(0.0);
        return (0.0);
    }
    mbs = initial;
    wcsnrtombs(buf, &wcp, (size_t) - 1, len + 1, &mbs);

    /* Let strtod() do most of the work for us. */
    val = strtod(buf, &end);

    /*
     * We only know where the number ended in the _multibyte_
     * representation of the string. If the caller wants to know
     * where it ended, count multibyte characters to find the
     * corresponding position in the wide char string.
     */
    if (endptr != NULL) {
        const char *decimal_point = localeconv()->decimal_point;
        /* The only valid multibyte char in a float converted by
           strtod/wcstod is the radix char.  What we do here is,
           figure out if the radix char was in the valid leading
           float sequence in the incoming string.  If so, the
           multibyte float string is strlen(radix char) - 1 bytes
           longer than the incoming wide char string has characters.
           To fix endptr, reposition end as if the radix char was
           just one byte long.  The resulting difference (end - buf)
           is then equivalent to the number of valid wide characters
           in the input string. */
        len = strlen(decimal_point);
        if (len > 1) {
            char *d = strstr(buf, decimal_point);
            if (d && d < end)
                end -= len - 1;
        }
        *endptr = (wchar_t *) nptr + (end - buf);
    }

    free(buf);

    RETURN(val);
    return (val);
}
