/*
 * $Id: wctype_jp2uc.c,v 1.0 2021-03-02 23:48:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#include "wctype_jp2uc.h"

wint_t
__jp2uc(wint_t c, int type)
{
    int index, adj;
    unsigned char byte1, byte2;
    wint_t ret;

    /* we actually use tables of EUCJP to Unicode.  For JIS, we simply
     note that EUCJP is essentially JIS with the top bits on in each
     byte and translate to EUCJP.  For SJIS, we do a translation to EUCJP before
     accessing the tables. */
    switch (type)
    {
    case JP_JIS:
        byte1 = (c >> 8) + 0x80;
        byte2 = (c & 0xff) + 0x80;
        break;
    case JP_EUCJP:
        byte1 = (c >> 8);
        byte2 = (c & 0xff);
        break;
    case JP_SJIS:
        byte1 = c >> 8;
        byte2 = c & 0xff;
        if (byte2 <= 0x9e)
        {
            adj = 0xa1 - 0x22;
            byte2 = (byte2 - 31) + 0xa1;
        }
        else
        {
            adj = 0xa1 - 0x21;
            byte2 = (byte2 - 126) + 0xa1;
        }
        if (byte1 <= 0x9f)
            byte1 = ((byte1 - 112) << 1) + adj;
        else
            byte1 = ((byte1 - 176) << 1) + adj;
        break;
    default:
        return WEOF;
    }

    /* find conversion in jp2uc arrays */

    /* handle larger ranges first */
    if (byte1 >= 0xb0 && byte1 <= 0xcf && c <= 0xcfd3)
    {
        index = (byte1 - 0xb0) * 0xfe + (byte2 - 0xa1);
        return b02cf[index];
    }
    else if (byte1 >= 0xd0 && byte1 <= 0xf4 && c <= 0xf4a6)
    {
        index = (byte1 - 0xd0) * 0xfe + (byte2 - 0xa1);
        return d02f4[index];
    }

    /* handle smaller ranges here */
    switch (byte1)
    {
    case 0xA1:
        return (wint_t)a1[byte2 - 0xa1];
    case 0xA2:
        ret = a2[byte2 - 0xa1];
        if (ret != 0)
            return (wint_t)ret;
        break;
    case 0xA3:
        if (a3[byte2 - 0xa1])
            return (wint_t)(0xff00 + (byte2 - 0xa0));
        break;
    case 0xA4:
        if (byte2 <= 0xf3)
            return (wint_t)(0x3000 + (byte2 - 0x60));
        break;
    case 0xA5:
        if (byte2 <= 0xf6)
            return (wint_t)(0x3000 + byte2);
        break;
    case 0xA6:
        ret = 0;
        if (byte2 <= 0xd8)
            ret = (wint_t)a6[byte2 - 0xa1];
        if (ret != 0)
            return ret;
        break;
    case 0xA7:
        ret = 0;
        if (byte2 <= 0xf1)
            ret = (wint_t)a7[byte2 - 0xa1];
        if (ret != 0)
            return ret;
        break;
    case 0xA8:
        if (byte2 <= 0xc0)
            return (wint_t)a8[byte2 - 0xa1];
        break;
    default:
        return WEOF;
    }

    return WEOF;
}