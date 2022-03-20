/*
 * $Id: iconv_close.c,v 1.0 2021-03-09 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <iconv.h>
#include <sys/iconvnls.h>
#include "conv.h"
#include "local.h"

int 
iconv_close(iconv_t cd)
{
    int res;
    iconv_conversion_t *ic = (iconv_conversion_t *)cd;

    if ((void *)cd == NULL || cd == (iconv_t)-1 || ic->data == NULL || (ic->handlers != &_iconv_null_conversion_handlers && ic->handlers != &_iconv_ucs_conversion_handlers))
    {
        __set_errno(EBADF);
        return -1;
    }

    res = (int)ic->handlers->close(ic->data);

    free((void *)cd);

    return res;
}