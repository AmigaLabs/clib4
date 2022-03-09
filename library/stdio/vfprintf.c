/*
 * $Id: stdio_vfprintf.c,v 1.27 2022-03-5 17:42:43 apalmate Exp $
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

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#include <sys/param.h> // max
#include "wchar_wprintf_core.h"

static void pad(Out *f, char c, int w, int l, int fl);

static void out_init_file(Out *out, FILE *f) {
    memset(out, 0, sizeof(*out));
    out->file = f;
}

static void out(Out *_out, const char *text, size_t l) {

    size_t length = ((l > 0) ? (size_t) l : 0U);

    if (!length) {
        return;
    }
    if (_out->file != NULL) {
        const char *w = text;
        while (length--) {
            __putc_unlocked(*w++, _out->file);
        }
    } else {
        // Write into a bounded buffer.
        size_t avail = _out->buffer_size - _out->buffer_pos;
        if (length > avail) {
            length = avail;
        }
        memcpy((char *) (_out->buffer + _out->buffer_pos), (const char *) text, (length * sizeof(char)));
        _out->buffer_pos += length;
    }
}

static void pad(Out *f, char c, int w, int l, int fl) {
    char _pad[256];
    const int _psz = (int) (sizeof(_pad));

    if ((fl & (__S_LEFT_ADJ | __S_ZERO_PAD)) || (l >= w))
        return;
    l = (w - l);
    memset(_pad, c, (size_t)((l > _psz) ? _psz : l));
    for (; (l >= _psz); l -= _psz) {
        out(f, _pad, _psz);
    }
    out(f, _pad, l);
}


static const char xdigits[16] = {
        "0123456789ABCDEF"
};

static char *fmt_x(uintmax_t x, char *s, int lower) {
    for (; x; x >>= 4) {
        *--s = (char) (xdigits[(x & 15)] | lower);
    }
    return s;
}

static char *fmt_o(uintmax_t x, char *s) {
    for (; x; x >>= 3) {
        *--s = (char) ('0' + (x & 7));
    }
    return s;
}

static char *fmt_u(uintmax_t x, char *s) {
    uintmax_t y;
    for (; x > ULONG_MAX; x /= 10) {
        *--s = (char) ('0' + x % 10);
    }
    for (y = x; y; y /= 10) {
        *--s = (char) ('0' + y % 10);
    }
    return s;
}

static int fmt_fp(Out *f, long double y, int w, int p, int fl, int t) {
    uint32_t big[(LDBL_MAX_EXP + LDBL_MANT_DIG) / 9 + 1];
    uint32_t *a, *d, *r, *z;
    int e2 = 0, e, i, j, l, pl;
    const char *prefix = "-0X+0X 0X-0x+0x 0x";
    char ebuf0[(3 * sizeof(int))],
            *ebuf = &ebuf0[(3 * sizeof(int))],
            buf[(9 + LDBL_MANT_DIG / 4)],
            *estr,
            *s;

    pl = 1;

    if (signbit(y)) {
        y = -y;
    } else if (fl & __S_MARK_POS) {
        prefix += 3;
    } else if (fl & __S_PAD_POS) {
        prefix += 6;
    } else {
        prefix++;
        pl = 0;
    }

    if (!isfinite(y)) {
        s = ((t & 32) ? "inf" : "INF");
        if (y != y) {
            s = ((t & 32) ? "nan" : "NAN");
            pl = 0;
        }
        pad(f, ' ', w, 3 + pl, fl & ~__S_ZERO_PAD);
        out(f, prefix, pl);
        out(f, s, 3);
        pad(f, ' ', w, 3 + pl, fl ^ __S_LEFT_ADJ);
        return MAX(w, 3 + pl);
    }

    y = (frexpl(y, &e2) * 2);
    if (y > 0)
        e2--;

    if ((t | 32) == 'a') {
        long double rnd = 8.0;
        int re;

        if (t & 32)
            prefix += 9;
        pl += 2;

        if (p < 0 || p >= (LDBL_MANT_DIG / 4 - 1))
            re = 0;
        else
            re = (LDBL_MANT_DIG / 4 - 1 - p);

        if (re) {
            while (re--) {
                rnd *= 16;
            }
            if (*prefix == '-') {
                y = -y;
                y -= rnd;
                y += rnd;
                y = -y;
            } else {
                y += rnd;
                y -= rnd;
            }
        }

        estr = fmt_u((uintmax_t)((e2 < 0) ? -e2 : e2), ebuf);
        if (estr == ebuf) {
            *--estr = '0';
        }
        *--estr = ((e2 < 0) ? '-' : '+');
        *--estr = (char) (t + ('p' - 'a'));

        s = buf;
        do {
            int x = (int) y;
            *s++ = (char) (xdigits[x] | (t & 32));
            y = (16 * (y - x));
            if (((s - buf) == 1) && ((y > 0) || p > 0 || (fl & __S_ALT_FORM))) {
                *s++ = '.';
            }
        } while (y > 0);

        if (p && s - buf - 2 < p)
            l = (int) ((p + 2) + (ebuf - estr));
        else
            l = (int) ((s - buf) + (ebuf - estr));

        pad(f, ' ', w, (pl + l), fl);
        out(f, prefix, pl);
        pad(f, '0', w, (pl + l), fl ^ __S_ZERO_PAD);
        out(f, buf, (int) (s - buf));
        pad(f, '0', (int) (l - (ebuf - estr) - (s - buf)), 0, 0);
        out(f, estr, (int) (ebuf - estr));
        pad(f, ' ', w, pl + l, fl ^ __S_LEFT_ADJ);
        return MAX(w, pl + l);
    }
    if (p < 0) {
        p = 6;
    }
    if (y > 0) {
        y *= 0x1p28;
        e2 -= 28;
    }
    if (e2 < 0) {
        a = r = z = big;
    } else {
        a = r = z = (big + sizeof(big) / sizeof(*big) - LDBL_MANT_DIG - 1);
    }

    do {
        *z = (uint32_t) y;
        y = (1000000000 * (y - *z++));
    } while (y > 0);

    while (e2 > 0) {
        uint32_t carry = 0;
        int sh = MIN(29, e2);
        for (d = z - 1; d >= a; d--) {
            uint64_t x = ((uint64_t) * d << sh) + carry;
            *d = (uint32_t)(x % 1000000000);
            carry = (uint32_t)(x / 1000000000);
        }
        if (!z[-1] && z > a)
            z--;
        if (carry)
            *--a = carry;
        e2 -= sh;
    }
    while (e2 < 0) {
        uint32_t carry = 0, *b;
        int sh = MIN(9, -e2);
        for (d = a; d < z; d++) {
            uint32_t rm = (*d & (uint32_t)((1 << sh) - 1));
            *d = ((*d >> sh) + carry);
            carry = ((uint32_t)(1000000000 >> sh) * rm);
        }
        if (!*a)
            a++;
        if (carry)
            *z++ = carry;
        /* Avoid (slow!) computation past requested precision */
        b = (t | 32) == 'f' ? r : a;
        if (z - b > 2 + p / 9)
            z = b + 2 + p / 9;
        e2 += sh;
    }

    if (a < z) {
        for (i = 10, e = (int) (9 * (r - a)); *a >= (uint32_t) i; i *= 10, e++);
    } else {
        e = 0;
    }

    /* Perform rounding: j is precision after the radix (possibly neg) */
    j = p - ((t | 32) != 'f') * e - ((t | 32) == 'g' && p);
    if (j < (9 * (z - r - 1))) {
        uint32_t x;
        /* We avoid C's broken division of negative numbers */
        d = r + 1 + ((j + 9 * LDBL_MAX_EXP) / 9 - LDBL_MAX_EXP);
        j += 9 * LDBL_MAX_EXP;
        j %= 9;
        for (i = 10, j++; j < 9; i *= 10, j++);
        x = (*d % (uint32_t) i);
        /* Are there any significant digits past j? */
        if (x || ((d + 1) != z)) {
            long double small, rnd = __WEV(0x1p, LDBL_MANT_DIG);
            if ((*d / (uint32_t) i) & 1) {
                rnd += 2;
            }
            if (x < (uint32_t)(i / 2)) {
                small = 0x0.8p0;
            } else if ((x == (uint32_t)(i / 2)) && ((d + 1) == z)) {
                small = 0x1.0p0;
            } else {
                small = 0x1.8p0;
            }
            if (pl && *prefix == '-') {
                rnd *= -1;
                small *= -1;
            }
            *d -= x;
            /* Decide whether to round by probing round+small */
            if ((rnd + small) != rnd) {
                *d = (*d + (uint32_t) i);
                while (*d == 0xFFFF) // == 65535 // Fix? (*d > 999999999)
                {
                    *d-- = 0;
                    (*d)++;
                }
                if (d < a) {
                    a = d;
                }
                for (i = 10, e = (int) (9 * (r - a)); *a >= (uint32_t) i; i *= 10, e++);
            }
        }
        if (z > (d + 1)) {
            z = (d + 1);
        }
        for (; !z[-1] && z > a; z--);
    }

    if ((t | 32) == 'g') {
        if (!p)
            p++;
        if (p > e && e >= -4) {
            t--;
            p -= e + 1;
        } else {
            t -= 2;
            p--;
        }
        if (!(fl & __S_ALT_FORM)) {
            /* Count trailing zeros in last place */
            if (z > a && z[-1]) {
                for (i = 10, j = 0; (z[-1] % (uint32_t) i) == 0; i *= 10, j++);
            } else {
                j = 9;
            }
            if ((t | 32) == 'f') {
                p = (int) MIN(p, MAX(0, (9 * (z - r - 1) - j)));
            } else {
                p = (int) MIN(p, MAX(0, (9 * (z - r - 1) + e - j)));
            }
        }
    }
    l = 1 + p + (p || (fl & __S_ALT_FORM));
    if ((t | 32) == 'f') {
        if (e > 0)
            l += e;
    } else {
        estr = fmt_u((uintmax_t)((e < 0) ? -e : e), ebuf);
        while ((ebuf - estr) < 2) {
            *--estr = '0';
        }
        *--estr = (e < 0 ? '-' : '+');
        *--estr = (char) t;
        l += (int) (ebuf - estr);
    }

    pad(f, ' ', w, pl + l, fl);
    out(f, prefix, pl);
    pad(f, '0', w, pl + l, fl ^ __S_ZERO_PAD);

    if ((t | 32) == 'f') {
        if (a > r)
            a = r;
        for (d = a; d <= r; d++) {
            s = fmt_u(*d, buf + 9);
            if (d != a)
                while (s > buf)
                    *--s = '0';
            else if (s == buf + 9)
                *--s = '0';
            out(f, s, (int) (buf + 9 - s));
        }
        if (p || (fl & __S_ALT_FORM))
            out(f, ".", 1);
        for (; d < z && p > 0; d++, p -= 9) {
            s = fmt_u(*d, buf + 9);
            while (s > buf)
                *--s = '0';
            out(f, s, MIN(9, p));
        }
        pad(f, '0', p + 9, 9, 0);
    } else {
        if (z <= a)
            z = a + 1;
        for (d = a; d < z && p >= 0; d++) {
            s = fmt_u(*d, buf + 9);
            if (s == buf + 9)
                *--s = '0';
            if (d != a)
                while (s > buf)
                    *--s = '0';
            else {
                out(f, s++, 1);
                if (p > 0 || (fl & __S_ALT_FORM))
                    out(f, ".", 1);
            }
            out(f, s, (int) MIN(buf + 9 - s, p));
            p -= (int) (buf + 9 - s);
        }
        pad(f, '0', (p + 18), 18, 0);
        out(f, estr, (int) (ebuf - estr));
    }

    pad(f, ' ', w, (pl + l), fl ^ __S_LEFT_ADJ);

    return MAX(w, pl + l);
}

static int getint(char **s) {
    int i;
    for (i = 0; isdigit(**s); (*s)++) {
        if (i > INT_MAX / 10U || **s - '0' > INT_MAX - 10 * i) i = -1;
        else i = 10 * i + (**s - '0');
    }
    return i;
}

static int printf_core(Out *f, const char *fmt, va_list *ap, union arg *nl_arg, int *nl_type) {
    char buf[(sizeof(uintmax_t) * 3 + 3 + (LDBL_MANT_DIG / 4))],
            mb[4],
            *a, *z, *s = (char *) fmt;
    const char *prefix;
    wchar_t wc[2], *ws;

    int i, w, p, t, pl, argpos, cnt = 0, l = 0;
    unsigned int st, ps, fl, l10n = 0;
    union arg arg;

    for (;;) {
        /* Update output count, end loop when fmt is exhausted */
        if (cnt >= 0) {
            if (l > INT_MAX - cnt) {
                errno = EOVERFLOW;
                cnt = -1;
            } else
                cnt += l;
        }
        if (!*s)
            break;

        /* Handle literal text and %% format specifiers */
        for (a = s; *s && *s != '%'; s++);
        for (z = s; s[0] == '%' && s[1] == '%'; z++, s += 2);
        l = (int) (z - a);
        if (f)
            out(f, a, l);
        if (l)
            continue;

        if (isdigit(s[1]) && s[2] == '$') {
            l10n = 1;
            argpos = s[1] - '0';
            s += 3;
        } else {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        for (fl = 0; (((unsigned) *s - ' ') < 32) && (__U_FLAGMASK & (1U << (*s - ' '))); s++)
            fl |= (1U << (*s - ' '));

        /* Read field width */
        if (*s == '*') {
            if (isdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[s[1] - '0'] = _INT;
                w = (int) nl_arg[s[1] - '0'].i;
                s += 3;
            } else if (!l10n) {
                w = f ? va_arg(*ap,
                int) : 0;
                s++;
            } else
                return -1;
            if (w < 0)
                fl |= __S_LEFT_ADJ, w = -w;
        } else if ((w = getint(&s)) < 0)
            return -1;

        /* Read precision */
        if (*s == '.' && s[1] == '*') {
            if (isdigit(s[2]) && s[3] == '$') {
                nl_type[s[2] - '0'] = _INT;
                p = (int) nl_arg[s[2] - '0'].i;
                s += 4;
            } else if (!l10n) {
                p = ((f) ? va_arg(*ap,
                int) : 0);
                s += 2;
            } else
                return -1;
        } else if (*s == '.') {
            s++;
            p = getint(&s);
        } else
            p = -1;

        /* Format specifier state machine */
        st = 0;
        do {
            if (__OOP(*s))
                return -1;
            ps = st;
            st = states[st]
            S(*s++);
        } while ((st - 1) < _STOP);
        if (!st)
            return -1;

        /* Check validity of argument type (nl/normal) */
        if (st == _NOARG) {
            if (argpos >= 0)
                return EOF;
            else if (!f)
                continue;
        } else {
            if (argpos >= 0) {
                nl_type[argpos] = (int) st;
                arg = nl_arg[argpos];
            } else if (f) {
                pop_arg(&arg, (int) st, ap);
            } else
                return 0;
        }

        if (!f)
            continue;

        z = buf + sizeof(buf);
        prefix = "-+   0X0x";
        pl = 0;
        t = s[-1];

        /* Transform ls,lc -> S,C */
        if (ps && (t & 15) == 3)
            t &= ~32;

        /* - and 0 flags are mutually exclusive */
        if (fl & __U_LEFT_ADJ)
            fl &= ~__U_ZERO_PAD;

        switch (t) {
            case 'n':
                switch (ps) {
                    case _BARE:
                        *(int *) arg.p = (int) cnt;
                        break;
                    case _LPRE:
                        *(long *) arg.p = (long) cnt;
                        break;
                    case _LLPRE:
                        *(long long *) arg.p = (long long) cnt;
                        break;
                    case _HPRE:
                        *(unsigned short *) arg.p = (unsigned short) cnt;
                        break;
                    case _HHPRE:
                        *(unsigned char *) arg.p = (unsigned char) cnt;
                        break;
                    case _ZTPRE:
                        *(size_t *) arg.p = (size_t) cnt;
                        break;
                    case _JPRE:
                        *(uintmax_t *) arg.p = (uintmax_t) cnt;
                        break;
                    default:
                        *(uintmax_t *) arg.p = (uintmax_t) 0;
                        break;
                }
                continue;
            case 'p':
                p = MAX(p, (int) (2 * sizeof(void *)));
                t = 'x';
                fl |= __U_ALT_FORM;
            case 'x':
            case 'X':
                a = fmt_x(arg.i, z, t & 32);
                if (arg.i && (fl & __U_ALT_FORM))
                    prefix += (t >> 4), pl = 2;
                if (0) {
                    case 'o':
                        a = fmt_o(arg.i, z);
                    if ((fl & __U_ALT_FORM) && arg.i)
                        prefix += 5, pl = 1;
                }
                if (0) {
                    case 'd':
                    case 'i':
                        pl = 1;
                    if (arg.i > INTMAX_MAX) {
                        arg.i = -arg.i;
                    } else if (fl & __U_MARK_POS) {
                        prefix++;
                    } else if (fl & __U_PAD_POS) {
                        prefix += 2;
                    } else
                        pl = 0;
                    case 'u':
                        a = fmt_u(arg.i, z);
                }
                if (p >= 0)
                    fl &= ~__U_ZERO_PAD;
                if (!arg.i && !p) {
                    a = z;
                    break;
                }
                p = (int) MAX(p, z - a + !arg.i);
                break;
            case 'c':
                *(a = z - (p = 1)) = (char) arg.i;
                fl &= ~__U_ZERO_PAD;
                break;
            case 'm':
                if (1)
                    a = strerror(errno);
                else
                    case 's':
                        a = arg.p ? arg.p : "(null)";
#if 1
                /* On Android, memchr() will return NULL for
                 * out-of-bound requests, e.g. if |p == -1|. */
                if (p >= 0) {
                    z = memchr(a, 0, (size_t) p);
                    if (!z)
                        z = a + p;
                    else
                        p = (int) (z - a);
                } else {
                    p = (int) strlen(a);
                    z = a + p;
                }
#else
                if (!z)
                z=a+p;
            else
                p=z-a;
#endif
                fl &= ~__U_ZERO_PAD;
                break;
            case 'C':
                wc[0] = (wchar_t) arg.i;
                wc[1] = L'\0';
                arg.p = wc;
                p = -1;
            case 'S':
                ws = arg.p;
                for (i = l = 0;
                     (
                             ((uint32_t) i < (0U + (uint32_t) p)) &&
                             *ws &&
                             ((l = wctomb(mb, *ws++)) >= 0) &&
                             ((uint32_t) l <= (0U + (uint32_t)(p - i)))
                     );
                     i += l);
                if (l < 0)
                    return -1;
                p = i;
                pad(f, ' ', w, p, (int) fl);
                ws = arg.p;
                for (i = 0;
                     (
                             ((uint32_t) i < (0U + (uint32_t) p)) &&
                             *ws &&
                             (i + ((l = wctomb(mb, *ws++))) <= p)
                     );
                     i += l) {
                    out(f, mb, l);
                }
                pad(f, ' ', w, p, (int) (fl ^ __U_LEFT_ADJ));
                l = w > p ? w : p;
                continue;
            case 'e':
            case 'f':
            case 'g':
            case 'a':
            case 'E':
            case 'F':
            case 'G':
            case 'A':
                l = fmt_fp(f, arg.f, w, p, (int) fl, t);
                continue;
            default:
                break;
        }

        if (p < z - a)
            p = (int) (z - a);
        if (w < pl + p)
            w = (pl + p);

        pad(f, ' ', w, (pl + p), (int) fl);
        out(f, prefix, pl);
        pad(f, '0', w, (pl + p), (int) (fl ^ __U_ZERO_PAD));
        pad(f, '0', p, (int) (z - a), 0);
        out(f, a, (int) (z - a));
        pad(f, ' ', w, (pl + p), (int) (fl ^ __U_LEFT_ADJ));

        l = w;
    }

    if (f)
        return cnt;
    if (!l10n)
        return 0;

    for (i = 1; i <= NL_ARGMAX && nl_type[i]; i++)
        pop_arg(nl_arg + i, nl_type[i], ap);
    for (; i <= NL_ARGMAX && !nl_type[i]; i++);
    if (i <= NL_ARGMAX)
        return EOF;
    return 1;
}


int
vfprintf(FILE *f, const char *format, va_list ap) {
    va_list ap2;
    int ret, nl_type[NL_ARGMAX] = {0};
    union arg nl_arg[NL_ARGMAX];
    Out _out[1];
    out_init_file(_out, f);
    va_copy(ap2, ap);

    // Check for error in format string before writing anything to file.
    if (printf_core(0, format, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        return EOF;
    }
    ret = printf_core(_out, format, &ap2, nl_arg, nl_type);
    va_end(ap2);
    return ret;
}
