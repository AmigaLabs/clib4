/*
 * $Id: iconv.c,v 1.0 2021-03-09 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <iconv.h>
#include <sys/iconvnls.h>
#include "conv.h"
#include "local.h"

size_t
iconv(iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
    iconv_conversion_t *ic = (iconv_conversion_t *)cd;

    if ((void *)cd == NULL || cd == (iconv_t)-1 || ic->data == NULL || (ic->handlers != &_iconv_null_conversion_handlers && ic->handlers != &_iconv_ucs_conversion_handlers))
    {
        __set_errno(EBADF);
        return (size_t)-1;
    }

    if (inbuf == NULL || *inbuf == NULL)
    {
        mbstate_t state_null = ICONV_ZERO_MB_STATE_T;

        if (!ic->handlers->is_stateful(ic->data, 1))
            return (size_t)0;

        if (outbuf == NULL || *outbuf == NULL)
        {
            /* Reset shift state */
            ic->handlers->set_state(ic->data, &state_null, 1);

            return (size_t)0;
        }

        if (outbytesleft != NULL)
        {
            mbstate_t state_save = ICONV_ZERO_MB_STATE_T;

            /* Save current shift state */
            ic->handlers->get_state(ic->data, &state_save, 1);

            /* Reset shift state */
            ic->handlers->set_state(ic->data, &state_null, 1);

            /* Get initial shift state sequence and it's length */
            ic->handlers->get_state(ic->data, &state_null, 1);

            if (*outbytesleft >= state_null.__count)
            {
                memcpy((void *)(*outbuf), (void *)&state_null, state_null.__count);

                *outbuf += state_null.__count;
                *outbytesleft -= state_null.__count;

                return (size_t)0;
            }

            /* Restore shift state if output buffer is too small */
            ic->handlers->set_state(ic->data, &state_save, 1);
        }

        __set_errno(E2BIG);
        return (size_t)-1;
    }

    if (*inbytesleft == 0)
    {
        __set_errno(EINVAL);
        return (size_t)-1;
    }

    if (*outbytesleft == 0 || *outbuf == NULL)
    {
        __set_errno(E2BIG);
        return (size_t)-1;
    }

    return ic->handlers->convert(ic->data, (const unsigned char **)inbuf, inbytesleft, (unsigned char **)outbuf, outbytesleft, 0);
}