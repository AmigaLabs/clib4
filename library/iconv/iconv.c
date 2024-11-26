/*
 * $Id: iconv.c,v 1.0 2021-03-09 12:04:25 clib4devs Exp $
*/


#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "iconv_priv.h"

/* Table of characters that appear in legacy 8-bit codepages,
 * limited to 1024 slots (10 bit indices). The first 256 entries
 * are elided since those characters are obviously all included. */
static const unsigned short legacy_chars[] = {
    #include "legacychars.h"
};

static const unsigned short jis0208[84][94] = {
    #include "jis0208.h"
};

static const unsigned short gb18030[126][190] = {
    #include "gb18030.h"
};

static const unsigned short big5[89][157] = {
    #include "big5.h"
};

static const unsigned short hkscs[] = {
    #include "hkscs.h"
};

static const unsigned short ksc[93][94] = {
    #include "ksc.h"
};

static const unsigned short rev_jis[] = {
    #include "revjis.h"
};

static unsigned
get_16(const unsigned char *s, int e) {
    e &= 1;
    return s[e] << 8 | s[1 - e];
}

static void
put_16(unsigned char *s, unsigned c, int e) {
    e &= 1;
    s[e] = c >> 8;
    s[1 - e] = c;
}

static unsigned
get_32(const unsigned char *s, int e) {
    e &= 3;
    return s[e] + 0U << 24 | s[e ^ 1] << 16 | s[e ^ 2] << 8 | s[e ^ 3];
}

static void
put_32(unsigned char *s, unsigned c, int e) {
    e &= 3;
    s[e ^ 0] = c >> 24;
    s[e ^ 1] = c >> 16;
    s[e ^ 2] = c >> 8;
    s[e ^ 3] = c;
}

static unsigned
legacy_map(const unsigned char *map, unsigned char type, unsigned c) {
    if (c < 4 * type) return c;
    unsigned x = c - 4 * type;
    x = map[x * 5 / 4] >> 2 * x % 8 | map[x * 5 / 4 + 1] << 8 - 2 * x % 8 & 1023;
    return x < 256 ? x : legacy_chars[x - 256];
}

static unsigned
uni_to_jis(unsigned c) {
    unsigned nel = sizeof rev_jis / sizeof *rev_jis;
    unsigned d, j, i, b = 0;
    for (;;) {
        i = nel / 2;
        j = rev_jis[b + i];
        d = jis0208[j / 256][j % 256];
        if (d == c) return j + 0x2121;
        else if (nel == 1) return 0;
        else if (c < d)
            nel /= 2;
        else {
            b += i;
            nel -= nel / 2;
        }
    }
}

size_t
iconv(iconv_t cd, char **in, size_t *inb, char **out, size_t *outb) {
    size_t x = 0;
    struct stateful_cd *scd = 0;
    if (!((size_t) cd & 1)) {
        scd = (void *) cd;
        cd = scd->base_cd;
    }
    unsigned to = extract_to(cd);
    unsigned from = extract_from(cd);
    const unsigned char *map = charmaps + from + 1;
    const unsigned char *tomap = charmaps + to + 1;
    mbstate_t st = {0};
    wchar_t wc;
    unsigned c, d;
    size_t k, l;
    int err;
    unsigned char type = charmaps[from];
    unsigned char totype = charmaps[to];

    if (!in || !*in || !*inb) return 0;

    for (; *inb; *in += l, *inb -= l) {
        c = *(unsigned char *) *in;
        l = 1;

        switch (type) {
            case UTF_8:
                if (c < 128) break;
                l = mbrtowc(&wc, *in, *inb, &st);
                if (l == (size_t) - 1) goto ilseq;
                if (l == (size_t) - 2) goto starved;
                c = wc;
                break;
            case US_ASCII:
                if (c >= 128) goto ilseq;
                break;
            case WCHAR_T:
                l = sizeof(wchar_t);
                if (*inb < l) goto starved;
                c = *(wchar_t *) *in;
                if (0) {
                    case UTF_32BE:
                    case UTF_32LE:
                        l = 4;
                    if (*inb < 4) goto starved;
                    c = get_32((void *) *in, type);
                }
                if (c - 0xd800u < 0x800u || c >= 0x110000u) goto ilseq;
                break;
            case UCS2BE:
            case UCS2LE:
            case UTF_16BE:
            case UTF_16LE:
                l = 2;
                if (*inb < 2) goto starved;
                c = get_16((void *) *in, type);
                if ((unsigned) (c - 0xdc00) < 0x400) goto ilseq;
                if ((unsigned) (c - 0xd800) < 0x400) {
                    if (type - UCS2BE < 2U) goto ilseq;
                    l = 4;
                    if (*inb < 4) goto starved;
                    d = get_16((void *) (*in + 2), type);
                    if ((unsigned) (d - 0xdc00) >= 0x400) goto ilseq;
                    c = ((c - 0xd7c0) << 10) + (d - 0xdc00);
                }
                break;
            case UCS2:
            case UTF_16:
                l = 0;
                if (!scd->state) {
                    if (*inb < 2) goto starved;
                    c = get_16((void *) *in, 0);
                    scd->state = type == UCS2
                                 ? c == 0xfffe ? UCS2LE : UCS2BE
                                 : c == 0xfffe ? UTF_16LE : UTF_16BE;
                    if (c == 0xfffe || c == 0xfeff)
                        l = 2;
                }
                type = scd->state;
                continue;
            case UTF_32:
                l = 0;
                if (!scd->state) {
                    if (*inb < 4) goto starved;
                    c = get_32((void *) *in, 0);
                    scd->state = c == 0xfffe0000 ? UTF_32LE : UTF_32BE;
                    if (c == 0xfffe0000 || c == 0xfeff)
                        l = 4;
                }
                type = scd->state;
                continue;
            case SHIFT_JIS:
                if (c < 128) break;
                if (c - 0xa1 <= 0xdf - 0xa1) {
                    c += 0xff61 - 0xa1;
                    break;
                }
                l = 2;
                if (*inb < 2) goto starved;
                d = *((unsigned char *) *in + 1);
                if (c - 129 <= 159 - 129) c -= 129;
                else if (c - 224 <= 239 - 224) c -= 193;
                else goto ilseq;
                c *= 2;
                if (d - 64 <= 158 - 64) {
                    if (d == 127) goto ilseq;
                    if (d > 127) d--;
                    d -= 64;
                } else if (d - 159 <= 252 - 159) {
                    c++;
                    d -= 159;
                }
                c = jis0208[c][d];
                if (!c) goto ilseq;
                break;
            case EUC_JP:
                if (c < 128) break;
                l = 2;
                if (*inb < 2) goto starved;
                d = *((unsigned char *) *in + 1);
                if (c == 0x8e) {
                    c = d;
                    if (c - 0xa1 > 0xdf - 0xa1) goto ilseq;
                    c += 0xff61 - 0xa1;
                    break;
                }
                c -= 0xa1;
                d -= 0xa1;
                if (c >= 84 || d >= 94) goto ilseq;
                c = jis0208[c][d];
                if (!c) goto ilseq;
                break;
            case ISO2022_JP:
                if (c >= 128) goto ilseq;
                if (c == '\033') {
                    l = 3;
                    if (*inb < 3) goto starved;
                    c = *((unsigned char *) *in + 1);
                    d = *((unsigned char *) *in + 2);
                    if (c != '(' && c != '$') goto ilseq;
                    switch (128 * (c == '$') + d) {
                        case 'B':
                            scd->state = 0;
                            continue;
                        case 'J':
                            scd->state = 1;
                            continue;
                        case 'I':
                            scd->state = 4;
                            continue;
                        case 128 + '@':
                            scd->state = 2;
                            continue;
                        case 128 + 'B':
                            scd->state = 3;
                            continue;
                    }
                    goto ilseq;
                }
                switch (scd->state) {
                    case 1:
                        if (c == '\\') c = 0xa5;
                        if (c == '~') c = 0x203e;
                        break;
                    case 2:
                    case 3:
                        l = 2;
                        if (*inb < 2) goto starved;
                        d = *((unsigned char *) *in + 1);
                        c -= 0x21;
                        d -= 0x21;
                        if (c >= 84 || d >= 94) goto ilseq;
                        c = jis0208[c][d];
                        if (!c) goto ilseq;
                        break;
                    case 4:
                        if (c - 0x60 < 0x1f) goto ilseq;
                        if (c - 0x21 < 0x5e) c += 0xff61 - 0x21;
                        break;
                }
                break;
            case GB2312:
                if (c < 128) break;
                if (c < 0xa1) goto ilseq;
                /* fallthrough */
            case GBK:
            case GB18030:
                if (c < 128) break;
                c -= 0x81;
                if (c >= 126) goto ilseq;
                l = 2;
                if (*inb < 2) goto starved;
                d = *((unsigned char *) *in + 1);
                if (d < 0xa1 && type == GB2312) goto ilseq;
                if (d - 0x40 >= 191 || d == 127) {
                    if (d - '0' > 9 || type != GB18030)
                        goto ilseq;
                    l = 4;
                    if (*inb < 4) goto starved;
                    c = (10 * c + d - '0') * 1260;
                    d = *((unsigned char *) *in + 2);
                    if (d - 0x81 > 126) goto ilseq;
                    c += 10 * (d - 0x81);
                    d = *((unsigned char *) *in + 3);
                    if (d - '0' > 9) goto ilseq;
                    c += d - '0';
                    c += 128;
                    for (d = 0; d <= c;) {
                        k = 0;
                        for (int i = 0; i < 126; i++)
                            for (int j = 0; j < 190; j++)
                                if (gb18030[i][j] - d <= c - d)
                                    k++;
                        d = c + 1;
                        c += k;
                    }
                    break;
                }
                d -= 0x40;
                if (d > 63) d--;
                c = gb18030[c][d];
                break;
            case BIG5:
                if (c < 128) break;
                l = 2;
                if (*inb < 2) goto starved;
                d = *((unsigned char *) *in + 1);
                if (d - 0x40 >= 0xff - 0x40 || d - 0x7f < 0xa1 - 0x7f) goto ilseq;
                d -= 0x40;
                if (d > 0x3e) d -= 0x22;
                if (c - 0xa1 >= 0xfa - 0xa1) {
                    if (c - 0x87 >= 0xff - 0x87) goto ilseq;
                    if (c < 0xa1) c -= 0x87;
                    else c -= 0x87 + (0xfa - 0xa1);
                    c = (hkscs[4867 + (c * 157 + d) / 16] >> (c * 157 + d) % 16) % 2 << 17
                        | hkscs[c * 157 + d];
                    /* A few HKSCS characters map to pairs of UCS
                     * characters. These are mapped to surrogate
                     * range in the hkscs table then hard-coded
                     * here. Ugly, yes. */
                    if (c / 256 == 0xdc) {
                        union {
                            char c[8];
                            wchar_t wc[2];
                        } tmp;
                        char *ptmp = tmp.c;
                        size_t tmpx = iconv(combine_to_from(to, find_charmap("utf8")),
                                            &(char *) {"\303\212\314\204"
                                                       "\303\212\314\214"
                                                       "\303\252\314\204"
                                                       "\303\252\314\214"
                                                       + c % 256}, &(size_t) {4},
                                            &ptmp, &(size_t) {sizeof tmp});
                        size_t tmplen = ptmp - tmp.c;
                        if (tmplen > *outb) goto toobig;
                        if (tmpx) x++;
                        memcpy(*out, &tmp, tmplen);
                        *out += tmplen;
                        *outb -= tmplen;
                        continue;
                    }
                    if (!c) goto ilseq;
                    break;
                }
                c -= 0xa1;
                c = big5[c][d] | (c == 0x27 && (d == 0x3a || d == 0x3c || d == 0x42)) << 17;
                if (!c) goto ilseq;
                break;
            case EUC_KR:
                if (c < 128) break;
                l = 2;
                if (*inb < 2) goto starved;
                d = *((unsigned char *) *in + 1);
                c -= 0xa1;
                d -= 0xa1;
                if (c >= 93 || d >= 94) {
                    c += (0xa1 - 0x81);
                    d += 0xa1;
                    if (c >= 93 || c >= 0xc6 - 0x81 && d > 0x52)
                        goto ilseq;
                    if (d - 'A' < 26) d = d - 'A';
                    else if (d - 'a' < 26) d = d - 'a' + 26;
                    else if (d - 0x81 < 0xff - 0x81) d = d - 0x81 + 52;
                    else goto ilseq;
                    if (c < 0x20) c = 178 * c + d;
                    else c = 178 * 0x20 + 84 * (c - 0x20) + d;
                    c += 0xac00;
                    for (d = 0xac00; d <= c;) {
                        k = 0;
                        for (int i = 0; i < 93; i++)
                            for (int j = 0; j < 94; j++)
                                if (ksc[i][j] - d <= c - d)
                                    k++;
                        d = c + 1;
                        c += k;
                    }
                    break;
                }
                c = ksc[c][d];
                if (!c) goto ilseq;
                break;
            default:
                if (!c) break;
                c = legacy_map(map, type, c);
                if (!c) goto ilseq;
        }

        switch (totype) {
            case WCHAR_T:
                if (*outb < sizeof(wchar_t)) goto toobig;
                *(wchar_t *) *out = c;
                *out += sizeof(wchar_t);
                *outb -= sizeof(wchar_t);
                break;
            case UTF_8:
                if (*outb < 4) {
                    char tmp[4];
                    k = wctomb(tmp, c);
                    if (*outb < k) goto toobig;
                    memcpy(*out, tmp, k);
                } else k = wctomb(*out, c);
                *out += k;
                *outb -= k;
                break;
            case US_ASCII:
                if (c > 0x7f)
                    subst:
                    x++, c = '*';
                /* fallthrough */
            default:
                if (*outb < 1) goto toobig;
                if (c < 256 && c == legacy_map(tomap, type, c)) {
                    revout:
                    if (*outb < 1) goto toobig;
                    *(*out)++ = c;
                    *outb -= 1;
                    break;
                }
                d = c;
                for (c = 4 * totype; c < 256; c++) {
                    if (d == legacy_map(tomap, type, c)) {
                        goto revout;
                    }
                }
                goto subst;
            case SHIFT_JIS:
                if (c < 128) goto revout;
                if (c == 0xa5) {
                    x++;
                    c = '\\';
                    goto revout;
                }
                if (c == 0x203e) {
                    x++;
                    c = '~';
                    goto revout;
                }
                if (c - 0xff61 <= 0xdf - 0xa1) {
                    c += 0xa1 - 0xff61;
                    goto revout;
                }
                c = uni_to_jis(c);
                if (!c) goto subst;
                if (*outb < 2) goto toobig;
                d = c % 256;
                c = c / 256;
                *(*out)++ = (c + 1) / 2 + (c < 95 ? 112 : 176);
                *(*out)++ = c % 2 ? d + 31 + d / 96 : d + 126;
                *outb -= 2;
                break;
            case EUC_JP:
                if (c < 128) goto revout;
                if (c - 0xff61 <= 0xdf - 0xa1) {
                    c += 0x0e00 + 0x21 - 0xff61;
                } else {
                    c = uni_to_jis(c);
                }
                if (!c) goto subst;
                if (*outb < 2) goto toobig;
                *(*out)++ = c / 256 + 0x80;
                *(*out)++ = c % 256 + 0x80;
                *outb -= 2;
                break;
            case ISO2022_JP:
                if (c < 128) goto revout;
                if (c - 0xff61 <= 0xdf - 0xa1 || c == 0xa5 || c == 0x203e) {
                    if (*outb < 7) goto toobig;
                    *(*out)++ = '\033';
                    *(*out)++ = '(';
                    if (c == 0xa5) {
                        *(*out)++ = 'J';
                        *(*out)++ = '\\';
                    } else if (c == 0x203e) {
                        *(*out)++ = 'J';
                        *(*out)++ = '~';
                    } else {
                        *(*out)++ = 'I';
                        *(*out)++ = c - 0xff61 + 0x21;
                    }
                    *(*out)++ = '\033';
                    *(*out)++ = '(';
                    *(*out)++ = 'B';
                    *outb -= 7;
                    break;
                }
                c = uni_to_jis(c);
                if (!c) goto subst;
                if (*outb < 8) goto toobig;
                *(*out)++ = '\033';
                *(*out)++ = '$';
                *(*out)++ = 'B';
                *(*out)++ = c / 256;
                *(*out)++ = c % 256;
                *(*out)++ = '\033';
                *(*out)++ = '(';
                *(*out)++ = 'B';
                *outb -= 8;
                break;
            case UCS2:
                totype = UCS2BE;
                /* fallthrough */
            case UCS2BE:
            case UCS2LE:
            case UTF_16:
            case UTF_16BE:
            case UTF_16LE:
                if (c < 0x10000 || totype - UCS2BE < 2U) {
                    if (c >= 0x10000) c = 0xFFFD;
                    if (*outb < 2) goto toobig;
                    put_16((void *) *out, c, totype);
                    *out += 2;
                    *outb -= 2;
                    break;
                }
                if (*outb < 4) goto toobig;
                c -= 0x10000;
                put_16((void *) *out, (c >> 10) | 0xd800, totype);
                put_16((void *) (*out + 2), (c & 0x3ff) | 0xdc00, totype);
                *out += 4;
                *outb -= 4;
                break;
            case UTF_32:
                totype = UTF_32BE;
                /* fallthrough */
            case UTF_32BE:
            case UTF_32LE:
                if (*outb < 4) goto toobig;
                put_32((void *) *out, c, totype);
                *out += 4;
                *outb -= 4;
                break;
        }
    }
    return x;
ilseq:
    err = EILSEQ;
    x = -1;
    goto end;
toobig:
    err = E2BIG;
    x = -1;
    goto end;
starved:
    err = EINVAL;
    x = -1;
    end:
    __set_errno(err);
    return x;
}
