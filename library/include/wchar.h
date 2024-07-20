/*
 * $Id: wchar.h,v 1.8 2008-04-30 14:34:03 clib4devs Exp $
*/

#ifndef _WCHAR_H
#define _WCHAR_H

#include <features.h>

#include <stddef.h>
#include <stdarg.h>
#include <time.h>
#include <locale.h>
#include <stdint.h>

__BEGIN_DECLS

#define WEOF (-1)

typedef long int wint_t;

/* Conversion state information.  */
typedef struct
{
   int __count;
   union
   {
      wint_t __wch;
      unsigned char __wchb[4];
   } __value; /* Value so far.  */
} _mbstate_t;

#ifndef _MBSTATE_T
#define _MBSTATE_T
typedef _mbstate_t mbstate_t;
#endif /* _MBSTATE_T */

typedef struct __sFILE FILE;


/****************************************************************************/

extern wint_t btowc(int c);
extern int wctob(wint_t c);

/****************************************************************************/

extern wchar_t *wcscat(wchar_t *dest, const wchar_t *src);
extern wchar_t *wcsncat(wchar_t *dest, const wchar_t *src, size_t n);
extern int wcscmp(const wchar_t *s1, const wchar_t *s2);
extern int wcsncmp(const wchar_t *s1, const wchar_t *s2, size_t n);
extern wchar_t *wcscpy(wchar_t *dest, const wchar_t *src);
extern wchar_t *wcsncpy(wchar_t *dest, const wchar_t *src, size_t n);
extern size_t wcslen(const wchar_t *s);
extern wchar_t *wcschr(const wchar_t *s, wchar_t c);
extern size_t wcsspn(const wchar_t *s, const wchar_t *set);
extern wchar_t *wcspbrk(const wchar_t *s, const wchar_t *set);
extern wchar_t *wcstok(wchar_t *str, const wchar_t *delim, wchar_t **ptr);
extern wchar_t *wcsstr(const wchar_t *big, const wchar_t *little);

extern int wcswidth(const wchar_t *pwcs, size_t n);
extern int wcwidth(const wchar_t wc);
/****************************************************************************/

extern double wcstod(const wchar_t *str, wchar_t **ptr);
extern long wcstol(const wchar_t *str, wchar_t **ptr, int base);
extern unsigned long wcstoul(const wchar_t *str, wchar_t **ptr, int base);
extern float wcstof (const wchar_t *nptr, wchar_t **endptr);

/****************************************************************************/

extern int wscoll(const wchar_t *s1, const wchar_t *s2);
extern size_t wcsxfrm(wchar_t *dest, const wchar_t *src, size_t len);

/****************************************************************************/

extern wchar_t *wmemchr(const wchar_t *ptr, wchar_t val, size_t len);
extern int wmemcmp(const wchar_t *ptr1, const wchar_t *ptr2, size_t len);
extern wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t len);
extern wchar_t *wmemmove(wchar_t *dest, const wchar_t *src, size_t len);
extern wchar_t *wmemset(wchar_t *ptr, int val, size_t len);

/****************************************************************************/

extern int fwide(FILE *stream, int orient);

/****************************************************************************/

extern wint_t fgetwc(FILE *stream);
extern wint_t getwc(FILE *stream);
extern wint_t getwchar(void);
extern wint_t ungetwc(wint_t wc, FILE *fp);

/****************************************************************************/

extern wchar_t *fgetws(wchar_t *s, int n, FILE *stream);

/****************************************************************************/

extern int fwscanf(FILE *stream, const wchar_t *format, ...);
extern int swscanf(const wchar_t *s, const wchar_t *format, ...);
extern int vfwscanf(FILE *f, const wchar_t *format, va_list ap);
extern int vswscanf(const wchar_t *s, const wchar_t *format, va_list ap);
extern int vwscanf(const wchar_t *s, va_list ap);
extern int wscanf(const wchar_t *format, ...);

/****************************************************************************/

extern wint_t fputwc(wchar_t c, FILE *stream);
extern wint_t putwc(wchar_t c, FILE *stream);
extern wint_t putwchar(wchar_t c);

/****************************************************************************/

extern int fputws(const wchar_t *s, FILE *stream);

/****************************************************************************/
//#if !defined(__STRICT_ANSI__) || (__STDC_VERSION__ >= 199901L)
extern int fwprintf(FILE *stream, const wchar_t *format, ...);
extern int swprintf(wchar_t *s, size_t l, const wchar_t *fmt, ...);
extern int vfwprintf(FILE *stream, const wchar_t *format, va_list arg);
extern int vswprintf(wchar_t *s, size_t maxlen, const wchar_t *format, va_list arg);
extern int vwprintf(const wchar_t *format, va_list arg);
extern int wprintf(const wchar_t *format, ...);
//#endif

extern size_t wcsftime(wchar_t *s, size_t maxsize, const wchar_t *format, const struct tm *timeptr);


#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
extern size_t mbrlen(const char *s, size_t n, mbstate_t *ps);
extern size_t mbrtowc(wchar_t *pwc, const char *src, size_t n, mbstate_t *ps);
extern int mbsinit(const mbstate_t *ps);
extern size_t mbsrtowcs(wchar_t *dst, const char **src, size_t len, mbstate_t *ps);

extern size_t wcrtomb(char *s, wchar_t wc, mbstate_t *ps);
extern int wcscoll(const wchar_t *ws1, const wchar_t *ws2);
extern size_t wcscspn(const wchar_t *s, const wchar_t *c);
extern wchar_t *wcsrchr(const wchar_t *ws, wchar_t wc);
extern size_t wcsrtombs(char *dst, const wchar_t **src, size_t len, mbstate_t *ps);
extern long long wcstoll(const wchar_t *str, wchar_t **ptr, int base);
extern unsigned long long wcstoull(const wchar_t *str, wchar_t **ptr, int base);
extern long double wcstold(const wchar_t *nptr, wchar_t **endptr);
#endif

#if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE) \
 || defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE)  || defined(_BSD_SOURCE)
extern size_t mbsnrtowcs(wchar_t *dst, const char **src, size_t nmc, size_t len, mbstate_t *ps);
extern size_t wcsnrtombs(char *dst, const wchar_t **src, size_t nwc, size_t len, mbstate_t *ps);
extern wchar_t *wcsdup(const wchar_t *src);
extern size_t wcsnlen (const wchar_t *src, size_t n);
extern wchar_t *wcpcpy (wchar_t *dst, const wchar_t *src);
extern wchar_t *wcpncpy (wchar_t *dst, const wchar_t *src, size_t len);
extern int wcscasecmp(const wchar_t *l, const wchar_t *r);
extern int wcscasecmp_l(const wchar_t *l, const wchar_t *r, locale_t loc);
extern int wcsncasecmp(const wchar_t *l, const wchar_t *r, size_t n);
extern int wcsncasecmp_l(const wchar_t *l, const wchar_t *r, size_t n, locale_t loc);
extern int wcscoll_l(const wchar_t *ws1, const wchar_t *ws2, locale_t loc);
extern size_t wcsxfrm_l(wchar_t *dest, const wchar_t *src, size_t len, locale_t loc);
#endif

__END_DECLS

#endif /* _WCHAR_H */
