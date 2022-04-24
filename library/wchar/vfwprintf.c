/*
 * $Id: wchar_vfwprintf.c,v 1.4 2021-02-05 00:41:13 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include "wchar_wprintf_core.h"

#include <wctype.h>

static void out_putwc(wchar_t wc, FOut *_out);
static int out_printf(FOut *_out, const char *format, ...);
static int out_error(FOut *_out);
static int getint(wchar_t **s);

size_t
wstring_wstocs(char dst[], size_t dsz, const wchar_t *src, size_t ssz) {
    if (!src) {
        __set_errno(EINVAL);
        return 0U;
    }

    ssz = ((!ssz) ? wcslen(src) : ssz);
    dsz = (((ssz * sizeof(wchar_t)) >= dsz) ? (dsz - 1) : dsz);
    if ((dsz = wcstombs(dst, src, dsz)) == 0) {
        return 0U;
    }
    dst[dsz] = '\0';
    return dsz;
}

size_t
wstring_cstows(wchar_t dst[], size_t dsz, const char *src, size_t ssz) {
    if (!src) {
        __set_errno(EINVAL);
        return 0U;
    }

    ssz = ((!ssz) ? strlen(src) : ssz);
    dsz = ((ssz >= dsz) ? (dsz - 1) : dsz);
    if ((dsz = mbstowcs(dst, src, dsz)) == 0) {
        return 0U;
    }
    dst[dsz] = L'\0';
    return dsz;
}

int
__wc_indelim(wchar_t wc, const wchar_t *delim) {
    while (*delim) {
        if (wc == *delim)
            return 1;
        delim++;
    }
    return 0;
}

static void
out_init_file(FOut *out, FILE *f) {
    memset(out, 0, sizeof(*out));
    out->file = f;
}

void
out_init_buffer(FOut *out, wchar_t *buffer, size_t buffer_size) {
    memset(out, 0, sizeof(*out));
    out->buffer = buffer;
    out->buffer_pos = 0;
    out->buffer_size = buffer_size;
}

static void
out(FOut *_out, const wchar_t *text, size_t length) {
    if (!length) {
        return;
    }
    if (_out->file != NULL) {
        wchar_t const *w = text;
        while (length--) {
            fputwc(*w++, _out->file);
        }
    } else {
        // Write into a bounded buffer.
        size_t avail = _out->buffer_size - _out->buffer_pos;
        if (length > avail) {
            length = avail;
        }
        memcpy((char *) (_out->buffer + _out->buffer_pos), (const char *) text, (length * sizeof(wchar_t)));
        _out->buffer_pos += length;
    }
}

static void
out_putwc(wchar_t wc, FOut *_out) {
    if (_out->file) {
        fputwc(wc, _out->file);
    } else if (_out->buffer_pos < _out->buffer_size) {
        _out->buffer[_out->buffer_pos++] = wc;
    }
}

static int
out_printf(FOut *_out, const char *format, ...) {
    va_list args;
    va_start(args, format);

    ENTER();

    if (_out->file) {
        int ret = vfprintf(_out->file, format, args);
        va_end(args);
        RETURN(ret);
        return ret;
    } else {
        // TODO(digit): Make this faster.
        // First, generate formatted byte output.
        size_t mb_len, wide_len;
        char *mb_buffer;
        wchar_t *wide_buffer;

        if (
                (!(mb_len = (size_t) vsnprintf(NULL, 0, format, args))) ||
                ((mb_buffer = malloc((mb_len + 1))) == NULL)) {
            va_end(args);
            RETURN(0);
            return 0;
        }

        (void) vsnprintf(mb_buffer, (mb_len + 1), format, args);

        // Then convert to wchar_t buffer.
        wide_len = mbstowcs(NULL, mb_buffer, mb_len);

        if (
                (wide_len == 0) || (wide_len == (size_t) - 1) ||
                ((wide_buffer = malloc((wide_len + 1) * sizeof(wchar_t))) == NULL)) {
            va_end(args);
            free(mb_buffer);
            RETURN(0);
            return 0;
        }

        (void) mbstowcs(wide_buffer, mb_buffer, mb_len);

        // Add to buffer.
        out(_out, wide_buffer, wide_len);
        // finished
        free(wide_buffer);
        free(mb_buffer);
        va_end(args);
        RETURN((int) wide_len);
        return (int) wide_len;
    }
    va_end(args);
}

static int
out_error(FOut *_out) {
    if (_out->file != NULL) {
        return ferror(_out->file);
    }
    return 0;
}

int
out_overflow(FOut *_out) {
    if (_out->file != NULL) {
        return feof(_out->file);
    }
    return (_out->buffer_pos >= _out->buffer_size);
}

static int
getint(wchar_t **s) {
    int i;

    for (i = 0; iswdigit(**s); (*s)++)
        i = 10 * i + (**s - '0');
    return i;
}

static const char sizeprefix['y' - 'a'] =
        {
                ['a' - 'a'] = 'L',
                ['e' - 'a'] = 'L',
                ['f' - 'a'] = 'L',
                ['g' - 'a'] = 'L',
                ['d' - 'a'] = 'j',
                ['i' - 'a'] = 'j',
                ['o' - 'a'] = 'j',
                ['u' - 'a'] = 'j',
                ['x' - 'a'] = 'j',
                ['p' - 'a'] = 'j'
        };

int
wprintf_core(FOut *f, const wchar_t *fmt, va_list *ap, union arg *nl_arg, int *nl_type) {
    wchar_t *s = (wchar_t *) fmt;
    unsigned int l10n = 0;
    int w, p;
    union arg arg;
    int argpos;
    unsigned st, ps;
    int cnt = 0, l = 0;
    int i;
    int t;
    char *bs;
    char charfmt[16];
    wchar_t wc;

    for (;;) {
        wchar_t *a, *z;
        unsigned int litpct, fl;
        /* Update output count, end loop when fmt is exhausted */
        if (cnt >= 0) {
            if (l > INT_MAX - cnt) {
                if (!out_error(f))
                    __set_errno(EOVERFLOW);
                cnt = -1;
            } else
                cnt += l;
        }
        if (!*s)
            break;

        /* Handle literal text and %% format specifiers */
        for (a = s; *s && *s != '%'; s++);
        litpct = (unsigned int) (wcsspn(s, (const wchar_t *) L"%") / 2); /* Optimize %%%% runs */
        z = s + litpct;
        s += 2 * litpct;
        l = (int) (z - a);
        if (f)
            out(f, a, (size_t) l);
        if (l)
            continue;

        if (iswdigit(s[1]) && (s[2] == '$')) {
            l10n = 1;
            argpos = (s[1] - '0');
            s += 3;
        } else {
            argpos = -1;
            s++;
        }

        /* Read modifier flags */
        for (fl = 0; ((((unsigned) *s - ' ') < 32) && (__U_FLAGMASK & (1U << (*s - ' ')))); s++)
            fl |= (1U << (*s - ' '));

        /* Read field width */
        if (*s == '*') {
            if (iswdigit(s[1]) && s[2] == '$') {
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
                fl |= __U_LEFT_ADJ, w = -w;
        } else if ((w = getint(&s)) < 0)
            return -1;

        /* Read precision */
        if (*s == '.' && s[1] == '*') {
            if (isdigit(s[2]) && s[3] == '$') {
                nl_type[s[2] - '0'] = _INT;
                p = (int) nl_arg[s[2] - '0'].i;
                s += 4;
            } else if (!l10n) {
                p = f ? va_arg(*ap,
                int) : 0;
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
            st = states[st]S(*s++);
        } while ((st - 1) < _STOP);
        if (!st)
            return -1;

        /* Check validity of argument type (nl/normal) */
        if (st == _NOARG) {
            if (argpos >= 0)
                return -1;
            else if (!f)
                continue;
        } else {
            if (argpos >= 0) {
                nl_type[argpos] = (int) st;
                arg = nl_arg[argpos];
            } else if (f)
                pop_arg(&arg, (int) st, ap);
            else
                return 0;
        }

        if (!f)
            continue;
        t = s[-1];
        if (ps && (t & 15) == 3)
            t &= ~32;

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
            case 'c':
                out_putwc(btowc((int) arg.i), f);
                l = 1;
                continue;
            case 'C':
                out_putwc((wchar_t) arg.i, f);
                l = 1;
                continue;
            case 'S':
                a = arg.p;
                z = wmemchr(a, 0, (size_t) p);
                if (!z)
                    z = a + p;
                else
                    p = (int) (z - a);
                if (w < p)
                    w = p;
                if (!(fl & __U_LEFT_ADJ)) {
                    out_printf(f, "%.*s", (w - p), "");
                }
                out(f, a, (size_t) p);
                if ((fl & __U_LEFT_ADJ)) {
                    out_printf(f, "%.*s", (w - p), "");
                }
                l = w;
                continue;
            case 's':
                bs = arg.p;
                if (p < 0)
                    p = INT_MAX;
                for (i = l = 0; ((l < p) && ((i = mbtowc(&wc, bs, MB_LEN_MAX)) > 0)); bs += i, l++);
                if (i < 0)
                    return -1;
                p = l;
                if (w < p)
                    w = p;
                if (!(fl & __U_LEFT_ADJ)) {
                    out_printf(f, "%.*s", w - p, "");
                }
                bs = arg.p;
                while (l--) {
                    i = mbtowc(&wc, bs, MB_LEN_MAX);
                    bs += i;
                    out_putwc(wc, f);
                }
                if ((fl & __U_LEFT_ADJ)) {
                    out_printf(f, "%.*s", w - p, "");
                }
                l = w;
                continue;
            default:
                break;
        }

        snprintf(charfmt, sizeof charfmt, "%%%s%s%s%s%s*.%c%c",
                 ((!(fl & __U_ALT_FORM)) ? "" : "#"),
                 ((!(fl & __U_MARK_POS)) ? "" : "+"),
                 ((!(fl & __U_LEFT_ADJ)) ? "" : "-"),
                 ((!(fl & __U_PAD_POS)) ? "" : " "),
                 ((!(fl & __U_ZERO_PAD)) ? "" : "0"),
                 sizeprefix[(t | 32) - 'a'], t);

        switch (t | 32) {
            case 'a':
            case 'e':
            case 'f':
            case 'g':
                l = out_printf(f, charfmt, w, p, arg.f);
                break;
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'p':
                l = out_printf(f, charfmt, w, p, arg.i);
                break;
            default:
                break;
        }
    }

    /* Put a \0 at the end */
    out_putwc('\0', f);

    if (f)
        return cnt;
    if (!l10n)
        return 0;

    for (i = 1; i <= NL_ARGMAX && nl_type[i]; i++)
        pop_arg(nl_arg + i, nl_type[i], ap);
    for (; i <= NL_ARGMAX && !nl_type[i]; i++);
    if (i <= NL_ARGMAX)
        return -1;
    return 1;
}

int
vfwprintf(FILE *f, const wchar_t *format, va_list ap) {
    va_list ap2;
    int ret, nl_type[NL_ARGMAX] = {0};
    union arg nl_arg[NL_ARGMAX];
    FOut _out[1];
    out_init_file(_out, f);
    va_copy(ap2, ap);

    ENTER();

    if (__check_abort_enabled)
        __check_abort();

    // Check for error in format string before writing anything to file.
    if (wprintf_core(0, format, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        RETURN(EOF);
        return EOF;
    }
    ret = wprintf_core(_out, format, &ap2, nl_arg, nl_type);

    va_end(ap2);

    RETURN(ret);
    return ret;
}
