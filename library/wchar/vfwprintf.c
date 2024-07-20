/*
 * $Id: wchar_vfwprintf.c,v 1.5 2024-07-12 00:41:13 clib4devs Exp $
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
static int out_printf(struct _clib4 *__clib4, FOut *_out, const char *format, ...);
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
    out->buffer_size = f->size;
    out->buffer_pos = f->position;
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
    if (!length || _out == NULL) {
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
    if (_out != NULL) {
        if (_out->file) {
            fputwc(wc, _out->file);
        } else if (_out->buffer_pos < _out->buffer_size) {
            _out->buffer[_out->buffer_pos++] = wc;
        }
    }
}

static int
out_printf(struct _clib4 *__clib4, FOut *_out, const char *format, ...) {
    if (_out == NULL)
        return 0;

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
                ((mb_buffer = __malloc_r(__clib4, (mb_len + 1))) == NULL)) {
            va_end(args);
            RETURN(0);
            return 0;
        }

        (void) vsnprintf(mb_buffer, (mb_len + 1), format, args);

        // Then convert to wchar_t buffer.
        wide_len = mbstowcs(NULL, mb_buffer, mb_len);

        if (
                (wide_len == 0) || (wide_len == (size_t) - 1) ||
                ((wide_buffer = __malloc_r(__clib4, (wide_len + 1) * sizeof(wchar_t))) == NULL)) {
            va_end(args);
            __free_r(__clib4, mb_buffer);
            RETURN(0);
            return 0;
        }

        (void) mbstowcs(wide_buffer, mb_buffer, mb_len);

        // Add to buffer.
        out(_out, wide_buffer, wide_len);
        // finished
        __free_r(__clib4, wide_buffer);
        __free_r(__clib4, mb_buffer);
        wide_buffer = NULL;
        mb_buffer = NULL;
        va_end(args);

        RETURN((int) wide_len);
        return (int) wide_len;
    }
    va_end(args);
}


int
out_overflow(FOut *_out) {
    if (_out) {
        if (_out->file != NULL) {
            return feof(_out->file);
        }
        return (_out->buffer_pos >= _out->buffer_size);
    }
    return -1;
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

static void pop_arg(union arg *arg, int type, va_list *ap) {
    switch (type) {
               case _PTR:	    arg->p = va_arg(*ap, void *);
        break; case _INT:	    arg->i = va_arg(*ap, int);
        break; case _UINT:	    arg->i = va_arg(*ap, unsigned int);
        break; case _LONG:	    arg->i = va_arg(*ap, long);
        break; case _ULONG:	    arg->i = va_arg(*ap, unsigned long);
        break; case _ULLONG:	arg->i = va_arg(*ap, unsigned long long);
        break; case _SHORT:	    arg->i = (short) va_arg(*ap, int);
        break; case _USHORT:	arg->i = (unsigned short) va_arg(*ap, int);
        break; case _CHAR:	    arg->i = (signed char) va_arg(*ap, int);
        break; case _UCHAR:	    arg->i = (unsigned char) va_arg(*ap, int);
        break; case _LLONG:	    arg->i = va_arg(*ap, long long);
        break; case _SIZET:	    arg->i = va_arg(*ap, size_t);
        break; case _IMAX:	    arg->i = va_arg(*ap, intmax_t);
        break; case _UMAX:	    arg->i = va_arg(*ap, uintmax_t);
        break; case _PDIFF:	    arg->i = va_arg(*ap, ptrdiff_t);
        break; case _UIPTR:	    arg->i = (uintptr_t) va_arg(*ap, void *);
        break; case _DBL:	    arg->f = va_arg(*ap, double);
        break; case _LDBL:	    arg->f = va_arg(*ap, long double);
    }
}

int
wprintf_core(struct _clib4 *__clib4, FOut *f, const wchar_t *fmt, va_list *ap, union arg *nl_arg, int *nl_type) {
    wchar_t *s = (wchar_t *) fmt;
    unsigned int l10n = 0;
    int w, p, xp;
    union arg arg;
    int argpos;
    unsigned st, ps;
    int cnt = 0, l = 0;
    int i;
    int t;
    char *bs;
    char charfmt[16] = {0};
    wchar_t wc;
    wchar_t *a, *z;
    unsigned int fl;

    for (;;) {
        /* This error is only specified for snprintf, but since it's
         * unspecified for other forms, do the same. Stop immediately
         * on overflow; otherwise %n could produce wrong results. */
        if (l > INT_MAX - cnt) {
            __set_errno_r(__clib4, EOVERFLOW);
            return -1;
        }

        /* Update output count, end loop when fmt is exhausted */
        cnt += l;
        if (!*s) break;

        /* Handle literal text and %% format specifiers */
        for (a = s; *s && *s != '%'; s++);
        for (z=s; s[0]=='%' && s[1]=='%'; z++, s+=2);
        if (z-a > INT_MAX-cnt) {
            __set_errno_r(__clib4, EOVERFLOW);
            return -1;
        }
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
        for (fl = 0; ((((unsigned) *s - ' ') < 32) && (FLAGMASK & (1U << (*s - ' ')))); s++)
            fl |= (1U << (*s - ' '));

        /* Read field width */
        if (*s == '*') {
            if (iswdigit(s[1]) && s[2] == '$') {
                l10n = 1;
                nl_type[s[1] - '0'] = _INT;
                w = (int) nl_arg[s[1] - '0'].i;
                s += 3;
            } else if (!l10n) {
                w = f ? va_arg(*ap, int) : 0;
                s++;
            } else {
                __set_errno_r(__clib4, EINVAL);
                return -1;
            }
            if (w < 0)
                fl |= LEFT_ADJ, w = -w;
        } else if ((w = getint(&s)) < 0) {
            __set_errno_r(__clib4, EOVERFLOW);
            return -1;
        }

        /* Read precision */
        if (*s == '.' && s[1] == '*') {
            if (isdigit(s[2]) && s[3] == '$') {
                nl_type[s[2] - '0'] = _INT;
                p = (int) nl_arg[s[2] - '0'].i;
                s += 4;
            } else if (!l10n) {
                p = f ? va_arg(*ap, int) : 0;
                s += 2;
            } else {
                __set_errno_r(__clib4, EINVAL);
                return -1;
            }
            xp = (p>=0);
        } else if (*s == '.') {
            s++;
            p = getint(&s);
            xp = 1;
        } else {
            p = -1;
            xp = 0;
        }

        /* Format specifier state machine */
        st = 0;
        do {
            if (__OOP(*s)){
                __set_errno_r(__clib4, EINVAL);
                return -1;
            }
            ps = st;
            st = states[st]S(*s++);
        } while ((st - 1) < _STOP);
        if (!st) {
            __set_errno_r(__clib4, EINVAL);
            return -1;
        }

        /* Check validity of argument type (nl/normal) */
        if (st == _NOARG) {
            if (argpos >= 0) {
                __set_errno_r(__clib4, EINVAL);
                return -1;
            }
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
                if (w < 1) w=1;
                if (w > 1 && !(fl & LEFT_ADJ)) {
                    out_printf(__clib4, f, "%*s", (w - 1), "");
                }
                out_putwc(btowc((int) arg.i), f);
                if (w > 1 && !(fl & LEFT_ADJ)) {
                    out_printf(__clib4, f, "%*s", (w - 1), "");
                }
                l = w;
                continue;
            case 'C':
                out_putwc((wchar_t) arg.i, f);
                l = 1;
                continue;
            case 'S':
                a = arg.p;
                z = a + wcsnlen(a, p<0 ? INT_MAX : p);
                if (p<0 && *z)  {
                    __set_errno_r(__clib4, EOVERFLOW);
                    return -1;
                }
                p = (int) (z - a);
                if (w < p)
                    w = p;
                if (!(fl & LEFT_ADJ)) {
                    out_printf(__clib4, f, "%*s", (w - p), "");
                }
                out(f, a, (size_t) p);
                if ((fl & LEFT_ADJ)) {
                    out_printf(__clib4, f, "%*s", (w - p), "");
                }
                l = w;
                continue;
            case 'm':
                arg.p = strerror(errno);
                /* fallthrough */
            case 's':
                if (!arg.p)
                    arg.p = (char *) "(null)";
                bs = arg.p;
                if (p < 0)
                    p = INT_MAX;
                for (i = l = 0; ((l < p) && ((i = mbtowc(&wc, bs, MB_LEN_MAX)) > 0)); bs += i, l++);
                if (i < 0)
                    return -1;
                if (p<0 && *bs)  {
                    __set_errno_r(__clib4, EOVERFLOW);
                    return -1;
                }
                p = l;
                if (w < p)
                    w = p;
                if (!(fl & LEFT_ADJ)) {
                    out_printf(__clib4, f, "%*s", w - p, "");
                }
                bs = arg.p;
                while (l--) {
                    i = mbtowc(&wc, bs, MB_LEN_MAX);
                    bs += i;
                    out_putwc(wc, f);
                }
                if ((fl & LEFT_ADJ)) {
                    out_printf(__clib4, f, "%*s", w - p, "");
                }
                l = w;
                continue;
            default:
                break;
        }

        if (xp && p<0) {
            __set_errno_r(__clib4, EOVERFLOW);
            return -1;
        }

        switch (t | 32) {
            case 'a':
            case 'e':
            case 'f':
            case 'g':
                snprintf(charfmt, sizeof charfmt, "%%%s%s%s%s%s*.*%c",
                         "#"+!(fl & ALT_FORM),
                         "+"+!(fl & MARK_POS),
                         "-"+!(fl & LEFT_ADJ),
                         " "+!(fl & PAD_POS),
                         "0"+!(fl & ZERO_PAD),
                         t);
                l = out_printf(__clib4, f, charfmt, w, p, arg.f);
                break;
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'p':
                snprintf(charfmt, sizeof charfmt, "%%%s%s%s%s%s*.*%c%c",
                         "#"+!(fl & ALT_FORM),
                         "+"+!(fl & MARK_POS),
                         "-"+!(fl & LEFT_ADJ),
                         " "+!(fl & PAD_POS),
                         "0"+!(fl & ZERO_PAD),
                         sizeprefix[(t|32)-'a'], t);
                l = out_printf(__clib4, f, charfmt, w, p, arg.i);
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
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    __check_abort_f(__clib4);

    // Check for error in format string before writing anything to file.
    if (wprintf_core(__clib4, 0, format, &ap2, nl_arg, nl_type) < 0) {
        va_end(ap2);
        RETURN(EOF);
        return EOF;
    }
    ret = wprintf_core(__clib4, _out, format, &ap2, nl_arg, nl_type);

    va_end(ap2);

    fflush(f);

    RETURN(ret);
    return ret;
}
