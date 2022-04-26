/*
 * $Id: wctype_wctype.c,v 1.4 2021-02-04 00:25:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

wctype_t
wctype(const char *property) {
    ENTER();

    wctype_t result = 0;

    switch (*property) {
        case 'a':
            if (!strcmp(property, "alnum"))
                result = WC_ALNUM;
            else if (!strcmp(property, "alpha"))
                result = WC_ALPHA;
            break;
        case 'b':
            if (!strcmp(property, "blank"))
                result = WC_BLANK;
            break;
        case 'c':
            if (!strcmp(property, "cntrl"))
                result = WC_CNTRL;
            break;
        case 'd':
            if (!strcmp(property, "digit"))
                result = WC_DIGIT;
            break;
        case 'g':
            if (!strcmp(property, "graph"))
                result = WC_GRAPH;
            break;
        case 'l':
            if (!strcmp(property, "lower"))
                result = WC_LOWER;
            break;
        case 'p':
            if (!strcmp(property, "print"))
                result = WC_PRINT;
            else if (!strcmp(property, "punct"))
                result = WC_PUNCT;
            break;
        case 's':
            if (!strcmp(property, "space"))
                result = WC_SPACE;
            break;
        case 'u':
            if (!strcmp(property, "upper"))
                result = WC_UPPER;
            break;
        case 'x':
            if (!strcmp(property, "xdigit"))
                result = WC_XDIGIT;
            break;
        default:
            result = 0;
            __set_errno(EINVAL);
            break;
    }

    RETURN(result);
    return result;
}
