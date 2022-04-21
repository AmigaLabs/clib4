/*
 * $Id: wchar_wcsnrtombs.c,v 1.0 2021-02-04 11:26:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
wcsnrtombs(char *dst, const wchar_t **src, size_t nwc, size_t len, mbstate_t *ps)
{
    char *ptr = dst;
    char buff[10] = {0};
    wchar_t *pwcs;
    size_t n;
    int i;

    ENTER();

    if (ps == NULL)
    {
        ps = &__global_clib2->wide_status->_wcsrtombs_state;
    }

    /* If no dst pointer, treat len as maximum possible value. */
    if (dst == NULL)
        len = (size_t)-1;

    n = 0;
    pwcs = (wchar_t *)(*src);

    while (n < len && nwc-- > 0)
    {
        int count = ps->__count;
        wint_t wch = ps->__value.__wch;
        int bytes = wcrtomb(buff, *pwcs, ps);
        if (bytes == -1)
        {
            __set_errno(EILSEQ);
            ps->__count = 0;
            return (size_t)-1;
        }
        if (n + bytes <= len)
        {
            n += bytes;
            if (dst)
            {
                for (i = 0; i < bytes; ++i)
                    *ptr++ = buff[i];
                ++(*src);
            }
            if (*pwcs++ == 0x00)
            {
                if (dst)
                    *src = NULL;
                ps->__count = 0;
                return n - 1;
            }
        }
        else
        {
            /* not enough room, we must back up state to before __WCTOMB call */
            ps->__count = count;
            ps->__value.__wch = wch;
            len = 0;
        }
    }

    return n;
}
