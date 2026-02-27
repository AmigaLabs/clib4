#ifndef _ICONV_PRIV_H
#define _ICONV_PRIV_H

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <iconv.h>

#define UTF_32BE    0300
#define UTF_16LE    0301
#define UTF_16BE    0302
#define UTF_32LE    0303
#define UCS2BE      0304
#define UCS2LE      0305
#define WCHAR_T     0306
#define US_ASCII    0307
#define UTF_8       0310
#define UTF_16      0312
#define UTF_32      0313
#define UCS2        0314
#define EUC_JP      0320
#define SHIFT_JIS   0321
#define ISO2022_JP  0322
#define GB18030     0330
#define GBK         0331
#define GB2312      0332
#define BIG5        0340
#define EUC_KR      0350

/* Definitions of charmaps. Each charmap consists of:
 * 1. Empty-string-terminated list of null-terminated aliases.
 * 2. Special type code or number of elided quads of entries.
 * 3. Character table (size determined by field 2), consisting
 *    of 5 bytes for every 4 characters, interpreted as 10-bit
 *    indices into the legacy_chars table. */

static const unsigned char charmaps[] =
        "utf8\0char\0\0\310"
        "wchart\0\0\306"
        "ucs2be\0\0\304"
        "ucs2le\0\0\305"
        "utf16be\0\0\302"
        "utf16le\0\0\301"
        "ucs4be\0utf32be\0\0\300"
        "ucs4le\0utf32le\0\0\303"
        "ascii\0usascii\0iso646\0iso646us\0\0\307"
        "utf16\0\0\312"
        "ucs4\0utf32\0\0\313"
        "ucs2\0\0\314"
        "eucjp\0\0\320"
        "shiftjis\0sjis\0\0\321"
        "iso2022jp\0\0\322"
        "gb18030\0\0\330"
        "gbk\0\0\331"
        "gb2312\0\0\332"
        "big5\0bigfive\0cp950\0big5hkscs\0\0\340"
        "euckr\0ksc5601\0ksx1001\0cp949\0\0\350"

#include "codepages.h"
;

struct stateful_cd {
    iconv_t base_cd;
    unsigned state;
};

extern int fuzzycmp(const unsigned char *a, const unsigned char *b);
extern size_t find_charmap(const void *name);
extern iconv_t combine_to_from(size_t t, size_t f);
extern size_t extract_from(iconv_t cd);
extern size_t extract_to(iconv_t cd);

#endif