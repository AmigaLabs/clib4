/*
 * $Id: iconv.c,v 1.0 2021-03-09 12:04:25 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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