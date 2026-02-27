/*
 * $Id: wchar_headers.h,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#define _WCHAR_HEADERS_H

/****************************************************************************/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <wchar.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifdef ERROR
#undef ERROR
#endif

typedef enum {
    ESCAPE,
    DOLLAR,
    BRACKET,
    AT,
    B,
    J,
    NUL,
    JIS_CHAR,
    OTHER,
    JIS_C_NUM
} JIS_CHAR_TYPE;

typedef enum {
    ASCII,
    JIS,
    A_ESC,
    A_ESC_DL,
    JIS_1,
    J_ESC,
    J_ESC_BR,
    INV,
    JIS_S_NUM
} JIS_STATE;

typedef enum {
    COPY_A,
    COPY_J1,
    COPY_J2,
    MAKE_A,
    NOOP,
    EMPTY,
    ERROR
} JIS_ACTION;

/* we override the mbstate_t __count field for more complex encodings and use it store a state value */
#define __state __count

/* valid values for wctrans_t */
#define WCT_TOLOWER 1
#define WCT_TOUPPER 2

/* valid values for wctype_t */
#define WC_ALNUM 1
#define WC_ALPHA 2
#define WC_BLANK 3
#define WC_CNTRL 4
#define WC_DIGIT 5
#define WC_GRAPH 6
#define WC_LOWER 7
#define WC_PRINT 8
#define WC_PUNCT 9
#define WC_SPACE 10
#define WC_UPPER 11
#define WC_XDIGIT 12

/* Japanese encoding types supported */
#define JP_JIS 1
#define JP_SJIS 2
#define JP_EUCJP 3

/* internal function to translate JP to Unicode */
extern wint_t __jp2uc(wint_t c, int type);

#define __NELE(a) (sizeof(a) / sizeof(a[0]))

#define __MAX(a, b) ((a) > (b) ? (a) : (b))
#define __MIN(a, b) ((a) < (b) ? (a) : (b))

#define __BITBOOL(x) (!(!(x)))
#define __BITSET(arg, mask) ((arg) | (1L << (mask)))
#define __BITCLR(arg, mask) ((arg) & ~(1L << (mask)))
#define __BITTST(arg, mask) __BITBOOL((arg) & (1L << (mask)))
#define __BITFLP(arg, mask) ((arg) ^ (1L << (mask)))

#define __WS_(x) L##x
#define __WEV_(A, B) A##B
#define __WEV(A, B) __WEV_(A, B)
#define __WEVFA_(_11, _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N
#define __WEVFA(...) __WEVFA_(__VA_ARGS__, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define __OOP(x) (((unsigned)(x) - 'A') > ('z' - 'A'))
#define __OOB(c, b) (((((b) >> 3) - 0x10) | (((b) >> 3) + ((int32_t)(c) >> 26))) & ~7)
#define R_(a, b) ((uint32_t)(((a == 0x80) ? (0x40U - b) : -a) << 23))
#define R(a, b) R_(__WEV(a, U), __WEV(b, U))
#define C(x) ((x < 2) ? 0xffffffff : (R(0x80, 0xc0) | x))
#define K(x) C((x + 16))
#define E(x) ((x == 0 ? R(0xa0, 0xc0) : x == 0xd ? R(0x80, 0xa0) : R(0x80, 0xc0)) | (R(0x80, 0xc0) >> 6) | x)
#define F(x) ((x >= 5 ? 0 : x == 0 ? R(0x90, 0xc0) : x == 4 ? R(0x80, 0xa0) : R(0x80, 0xc0)) | (R(0x80, 0xc0) >> 6) | (R(0x80, 0xc0) >> 12) | x)

#define __SA 0xc2u
#define __SB 0xf4u

/* Arbitrary encoding for representing code units instead of characters. */
#define CODEUNIT(c) (0xdfff & (signed char)(c))
#define IS_CODEUNIT(c) ((unsigned)(c)-0xdf80 < 0x80)

#define ESC_CHAR 0x1b

/* functions used to support SHIFT_JIS, EUC-JP, and JIS multibyte encodings */
int _issjis1(int c);
int _issjis2(int c);
int _iseucjp(int c);
int _isjis(int c);

#define _issjis1(c) (((c) >= 0x81 && (c) <= 0x9f) || ((c) >= 0xe0 && (c) <= 0xef))
#define _issjis2(c) (((c) >= 0x40 && (c) <= 0x7e) || ((c) >= 0x80 && (c) <= 0xfc))
#define _iseucjp(c) ((c) >= 0xa1 && (c) <= 0xfe)
#define _isjis(c) ((c) >= 0x21 && (c) <= 0x7e)

#define __PSEP '/'

extern const uint32_t bittab[];

#define __AUTO(x) __attribute__((cleanup(x)))

/*
 * Set the orientation for a stream. If o > 0, the stream has wide-
 * orientation. If o < 0, the stream has byte-orientation.
 */
#define ORIENT(fp, ori)                    \
  do                                \
    {                                \
      if (!((fp)->_flags & __SORD))    \
    {                            \
      (fp)->_flags |= __SORD;                \
      if (ori > 0)                        \
        (fp)->_flags2 |= __SWID;                \
      else                            \
        (fp)->_flags2 &= ~__SWID;                \
    }                            \
    }                                \
  while (0)


/* Functions used in wgetwc and wgetws */
wint_t __fgetwc_unlocked_internal(struct _clib4 *__clib4, FILE *f);
wint_t __fgetwc_unlocked(struct _clib4 *__clib4, FILE *f);

#endif /* _WCHAR_HEADERS_H */
