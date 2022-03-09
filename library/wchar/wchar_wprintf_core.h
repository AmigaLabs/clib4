#ifndef _WPRINTF_CORE_H
#define _WPRINTF_CORE_H

#include <stdio.h>

enum {
	_BARE, _LPRE, _LLPRE, _HPRE, _HHPRE, _BIGLPRE,
	_ZTPRE, _JPRE,
	_STOP,
	_PTR, _INT, _UINT, _ULLONG,
#if 0
	_LONG, _ULONG,
#else
#define _LONG _INT
#define _ULONG _UINT
#endif
	_SHORT, _USHORT, _CHAR, _UCHAR,
	_LLONG, _SIZET, _IMAX, _UMAX, _PDIFF, _UIPTR,
	_DBL, _LDBL,
	_NOARG,
	_MAXSTATE
};

typedef struct
{
    FILE *file;
    wchar_t *buffer;
    size_t buffer_pos;
    size_t buffer_size;
} FOut;

#define S(x) [(x)-'A']

static const unsigned char states[]['z'-'A'+1] = {
	{ /* 0: bare types */
		S('d') = _INT, S('i') = _INT,
		S('o') = _UINT, S('u') = _UINT, S('x') = _UINT, S('X') = _UINT,
		S('e') = _DBL, S('f') = _DBL, S('g') = _DBL, S('a') = _DBL,
		S('E') = _DBL, S('F') = _DBL, S('G') = _DBL, S('A') = _DBL,
		S('c') = _CHAR, S('C') = _INT,
		S('s') = _PTR, S('S') = _PTR, S('p') = _UIPTR, S('n') = _PTR,
		S('m') = _NOARG,
		S('l') = _LPRE, S('h') = _HPRE, S('L') = _BIGLPRE,
		S('z') = _ZTPRE, S('j') = _JPRE, S('t') = _ZTPRE,
	}, { /* 1: l-prefixed */
		S('d') = _LONG, S('i') = _LONG,
		S('o') = _ULONG, S('u') = _ULONG, S('x') = _ULONG, S('X') = _ULONG,
		S('e') = _DBL, S('f') = _DBL, S('g') = _DBL, S('a') = _DBL,
		S('E') = _DBL, S('F') = _DBL, S('G') = _DBL, S('A') = _DBL,
		S('c') = _INT, S('s') = _PTR, S('n') = _PTR,
		S('l') = _LLPRE,
	}, { /* 2: ll-prefixed */
		S('d') = _LLONG, S('i') = _LLONG,
		S('o') = _ULLONG, S('u') = _ULLONG,
		S('x') = _ULLONG, S('X') = _ULLONG,
		S('n') = _PTR,
	}, { /* 3: h-prefixed */
		S('d') = _SHORT, S('i') = _SHORT,
		S('o') = _USHORT, S('u') = _USHORT,
		S('x') = _USHORT, S('X') = _USHORT,
		S('n') = _PTR,
		S('h') = _HHPRE,
	}, { /* 4: hh-prefixed */
		S('d') = _CHAR, S('i') = _CHAR,
		S('o') = _UCHAR, S('u') = _UCHAR,
		S('x') = _UCHAR, S('X') = _UCHAR,
		S('n') = _PTR,
	}, { /* 5: L-prefixed */
		S('e') = _LDBL, S('f') = _LDBL, S('g') = _LDBL, S('a') = _LDBL,
		S('E') = _LDBL, S('F') = _LDBL, S('G') = _LDBL, S('A') = _LDBL,
		S('n') = _PTR,
	}, { /* 6: z- or t-prefixed (assumed to be same size) */
		S('d') = _PDIFF, S('i') = _PDIFF,
		S('o') = _SIZET, S('u') = _SIZET,
		S('x') = _SIZET, S('X') = _SIZET,
		S('n') = _PTR,
	}, { /* 7: j-prefixed */
		S('d') = _IMAX, S('i') = _IMAX,
		S('o') = _UMAX, S('u') = _UMAX,
		S('x') = _UMAX, S('X') = _UMAX,
		S('n') = _PTR,
	}
};

#define OOB(x) ((unsigned)(x)-'A' > 'z'-'A')

/* Convenient bit representation for modifier flags, which all fall
 * within 31 codepoints of the space character.
 * vfprintf vfwprintf
 */

#define __M_ALT_FORM(_T) (_T << ('#' - ' '))
#define __M_ZERO_PAD(_T) (_T << ('0' - ' '))
#define __M_LEFT_ADJ(_T) (_T << ('-' - ' '))
#define __M_PAD_POS(_T) (_T << (' ' - ' '))
#define __M_MARK_POS(_T) (_T << ('+' - ' '))
#define __M_GROUPED(_T) (_T << ('\'' - ' '))

#define __U_ALT_FORM __M_ALT_FORM(1U)
#define __U_ZERO_PAD __M_ZERO_PAD(1U)
#define __U_LEFT_ADJ __M_LEFT_ADJ(1U)
#define __U_PAD_POS __M_PAD_POS(1U)
#define __U_MARK_POS __M_MARK_POS(1U)
#define __U_GROUPED __M_GROUPED(1U)

#define __S_ALT_FORM __M_ALT_FORM(1)
#define __S_ZERO_PAD __M_ZERO_PAD(1)
#define __S_LEFT_ADJ __M_LEFT_ADJ(1)
#define __S_PAD_POS __M_PAD_POS(1)
#define __S_MARK_POS __M_MARK_POS(1)
#define __S_GROUPED __M_GROUPED(1)

#define __U_FLAGMASK (__U_ALT_FORM | __U_ZERO_PAD | __U_LEFT_ADJ | __U_PAD_POS | __U_MARK_POS | __U_GROUPED)
#define __S_FLAGMASK (__S_ALT_FORM | __S_ZERO_PAD | __S_LEFT_ADJ | __S_PAD_POS | __S_MARK_POS | __S_GROUPED)

union arg
{
	uintmax_t i;
    long double f;
	void *p;
};

void pop_arg(union arg *arg, int type, va_list *ap);

void out_init_file(FOut *out, FILE *f);
int wprintf_core(FOut *f, const wchar_t *fmt, va_list *ap, union arg *nl_arg, int *nl_type);
int out_overflow(FOut *_out);
void out_init_buffer(FOut *out, wchar_t *buffer, size_t buffer_size);
int __wc_indelim(wchar_t wc, const wchar_t *delim);
size_t wstring_wstocs(char dst[], size_t dsz, const wchar_t *src, size_t ssz);
size_t wstring_cstows(wchar_t dst[], size_t dsz, const char *src, size_t ssz);

#endif