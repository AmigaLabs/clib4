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
wctype(const char *property)
{
    ENTER();

    switch (*property)
	{
        case 'a':
            if (!strcmp(property, "alnum"))
                return WC_ALNUM;
            else if (!strcmp(property, "alpha"))
                return WC_ALPHA;
            break;
        case 'b':
            if (!strcmp(property, "blank"))
                return WC_BLANK;
            break;
        case 'c':
            if (!strcmp(property, "cntrl"))
                return WC_CNTRL;
            break;
        case 'd':
            if (!strcmp(property, "digit"))
                return WC_DIGIT;
            break;
        case 'g':
            if (!strcmp(property, "graph"))
                return WC_GRAPH;
            break;
        case 'l':
            if (!strcmp(property, "lower"))
                return WC_LOWER;
            break;
        case 'p':
            if (!strcmp(property, "print"))
                return WC_PRINT;
            else if (!strcmp(property, "punct"))
                return WC_PUNCT;
            break;
        case 's':
            if (!strcmp(property, "space"))
                return WC_SPACE;
            break;
        case 'u':
            if (!strcmp(property, "upper"))
                return WC_UPPER;
            break;
        case 'x':
            if (!strcmp(property, "xdigit"))
                return WC_XDIGIT;
            break;
	}

	/* otherwise invalid */
	__set_errno(EINVAL);
	return 0;
}
