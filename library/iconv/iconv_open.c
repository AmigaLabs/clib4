/*
 * $Id: iconv_open.c,v 1.0 2021-03-09 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <iconv.h>
#include <sys/iconvnls.h>
#include "conv.h"

iconv_t
iconv_open(const char *to, const char *from)
{
    iconv_conversion_t *ic;

    if (to == NULL || from == NULL || *to == '\0' || *from == '\0')
    {
        __set_errno(EINVAL);
        return (iconv_t)-1;
    }

    if ((to = (const char *)_iconv_resolve_encoding_name(to)) == NULL)
    {
        __set_errno(EINVAL);
        return (iconv_t)-1;
    }

    if ((from = (const char *)_iconv_resolve_encoding_name(from)) == NULL)
    {
        free((void *)to);
        __set_errno(EINVAL);
        return (iconv_t)-1;
    }

    ic = (iconv_conversion_t *)malloc(sizeof(iconv_conversion_t));
    if (ic == NULL)
        return (iconv_t)-1;

    /* Select which conversion type to use */
    if (strcmp(from, to) == 0)
    {
        /* Use null conversion */
        ic->handlers = &_iconv_null_conversion_handlers;
        ic->data = ic->handlers->open(to, from);
    }
    else
    {
        /* Use UCS-based conversion */
        ic->handlers = &_iconv_ucs_conversion_handlers;
        ic->data = ic->handlers->open(to, from);
    }

    free((void *)to);
    free((void *)from);

    if (ic->data == NULL)
    {
        free((void *)ic);
        __set_errno(EINVAL);
        return (iconv_t)-1;
    }

    return (void *)ic;
}
