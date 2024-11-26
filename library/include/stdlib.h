/*
 * $Id: stdlib.h,v 1.19 2008-04-30 14:34:03 clib4devs Exp $
*/

#ifndef _STDLIB_H
#define _STDLIB_H

#include <features.h>

#include <stddef.h>
#include <wchar.h>
#include <alloca.h>
#include <sys/types.h>

/* Make the "restrict" qualifier work well with 'C' compilers that do
   not support it. We do assume that __STDC__ is defined, though. */
#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
#define restrict
#endif /* !__STDC_VERSION__ || __STDC_VERSION__ < 199901L */


__BEGIN_DECLS

/* Maximum number of bytes in a multibyte character */
extern  int *__mb_cur_max(void);
#define MB_CUR_MAX (*__mb_cur_max())

#define __program_name (*__getprogname())
extern const char *__getprogname(void);

/****************************************************************************/

/* The maximum value that can be returned by the rand() function */
#define RAND_MAX 2147483647

/****************************************************************************/

/* Return values to be passed to exit() */
#define EXIT_FAILURE 20
#define EXIT_SUCCESS 0

/****************************************************************************/

/* Data structures used by the div() and ldiv() functions */
typedef struct
{
	int quot;
	int rem;
} div_t;
typedef struct
{
	long quot;
	long rem;
} ldiv_t;

extern int mblen(const char *s, size_t n);
extern int mbtowc(wchar_t *pwc, const char *s, size_t n);
extern int _mbtowc_r(wchar_t *pwc, const char *s, size_t n, _mbstate_t *st);
extern size_t _mbstowcs_r(wchar_t *pwcs, const char *s, size_t n, mbstate_t *state);

extern int wctomb(char *s, wchar_t wchar);
extern int _wctomb_r(char *s, wchar_t wchar, mbstate_t *state);
extern size_t mbstowcs(wchar_t *ws, const char *s, size_t wn);
extern size_t wcstombs(char *s, const wchar_t *pwcs, size_t n);
extern size_t _wcstombs_r(char *s, const wchar_t *pwcs, size_t n, mbstate_t *state);

extern void *malloc(size_t size);
extern void *calloc(size_t num_elements, size_t element_size);
extern void free(void *ptr);
extern void *realloc(void *ptr, size_t size);
extern void *valloc(size_t size);
extern void *aligned_alloc(size_t alignment, size_t size);
extern int posix_memalign(void **memptr, size_t alignment, size_t size);

extern int abs(int x);
extern long labs(long x);
extern div_t div(int n, int d);
extern ldiv_t ldiv(long n, long d);

extern int rand(void);
extern void srand(unsigned int seed);

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) \
 || defined(_BSD_SOURCE)
extern long random(void);
extern void srandom(unsigned int seed);
extern char *initstate(unsigned int seed, char *state, size_t size);
extern char *setstate(const char *state);
extern char *l64a(long x0);
extern long a64l(const char *s);
extern void setkey(const char *key);

extern double drand48(void);
extern double erand48(unsigned short subi[3]);
extern long int lrand48(void);
extern long int nrand48(unsigned short subi[3]);
extern long mrand48(void);
extern long jrand48(unsigned short subi[3]);
extern void srand48(long);
extern unsigned short *seed48(unsigned short subi[3]);
extern void lcong48(unsigned short subi[7]);

#endif

extern int system(const char *command);
extern void exit(int status);
extern void abort(void);

extern int atexit(void (*)(void));
extern int __cxa_atexit(void (*func) (void *), void *arg, void *d);
extern void __cxa_finalize(void *d);
extern char *getenv(const char *name);
extern void *bsearch(const void *key, const void *base, size_t count, size_t size, int (*compare)(const void *key, const void *value));
extern void qsort(void *base, size_t count, size_t size, int (*compare)(const void *element1, const void *element2));

extern double strtod(const char *str, char **ptr);
extern long strtol(const char *str, char **ptr, int base);
extern unsigned long strtoul(const char *str, char **ptr, int base);
extern quad_t strtoq(const char *nptr, char **endptr, register int base);
extern u_quad_t strtouq(const char *nptr, char **endptr, int base);

extern double strtod_l (const char *__restrict, char **__restrict, locale_t);
extern float strtof_l (const char *__restrict, char **__restrict, locale_t);
extern long double strtold_l (const char *__restrict, char **__restrict, locale_t);
extern long strtol_l (const char *__restrict, char **__restrict, int, locale_t);
extern unsigned long strtoul_l (const char *__restrict, char **__restrict, int, locale_t __loc);
extern long long strtoll_l (const char *__restrict, char **__restrict, int, locale_t);
extern unsigned long long strtoull_l (const char *__restrict, char **__restrict, int, locale_t __loc);

extern double atof(const char *str);
extern int atoi(const char *str);
extern long atol(const char *str);

extern char *itoa(int value, char *buffer, int base);
extern char *lltoa(int64_t ll, char *buffer, int radix);

extern void _exit(int status);
extern int rand_r(unsigned int *seed);

extern char *mktemp(char *name_template);
extern int mkstemp(char *name_template);
extern int mkostemp(char *name, int flags);
extern int mkostemps(char *name, int len, int flags);
extern int unsetenv(const char *name);
extern int setenv(const char *name, const char *value, int overwrite);
extern int putenv(const char *string);

extern char *mkdtemp(char *name_template);
extern const char *getexecname(void);

extern long long strtoll(const char *str, char **ptr, int base);
extern long double strtold(const char *str, char **ptr);
extern unsigned long long strtoull(const char *str, char **ptr, int base);
extern long long atoll(const char *str);

typedef struct
{
	long long quot;
	long long rem;
} lldiv_t;

extern long long llabs(long long x);
extern lldiv_t lldiv(long long n, long long d);

extern float strtof(const char *str, char **ptr);

#ifndef __STRICT_ANSI__
/* the following strtodf interface is deprecated...use strtof instead */
# ifndef strtodf 
#  define strtodf strtof
# endif

extern float atoff(const char *nptr);
#endif
extern void _Exit(int status);

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || defined(_DEFAULT_SOURCE)
extern char *ecvt(double x, int n, int *dp, int *sign);
extern char *fcvt(double x, int n, int *dp, int *sign);
extern char *gcvt(double x, int n, char *b);
extern char *secure_getenv(const char *name);
extern void *reallocarray(void *ptr, size_t m, size_t n);
extern void qsort_r(void *, size_t, size_t, int (*)(const void *, const void *, void *), void *);
#endif

extern uint32_t arc4random(void);
extern void arc4random_buf(void *buf, size_t nbytes);
extern uint32_t arc4random_uniform(uint32_t upper_bound);
extern void arc4random_stir(void);
extern void arc4random_addrandom(unsigned char *dat, int datlen);

#ifdef __SPE__
struct _clib4;
extern int16_t atosfix16(const char *__str);
extern int16_t _atosfix16_r(struct _clib4 *, const char *__str);
extern int32_t atosfix32(const char *__str);
extern int32_t _atosfix32_r(struct _clib4 *, const char *__str);
extern int64_t atosfix64(const char *__str);
extern int64_t _atosfix64_r(struct _clib4 *, const char *__str);

extern uint16_t atoufix16(const char *__str);
extern uint16_t _atoufix16_r(struct _clib4 *, const char *__str);
extern uint32_t atoufix32(const char *__str);
extern uint32_t _atoufix32_r(struct _clib4 *, const char *__str);
extern uint64_t atoufix64(const char *__str);
extern uint64_t _atoufix64_r(struct _clib4 *, const char *__str);

extern int16_t strtosfix16(const char *__str, char **__endptr);
extern int16_t _strtosfix16_r(struct _clib4 *, const char *__str, char **__endptr);
extern int32_t strtosfix32(const char *__str, char **__endptr);
extern int32_t _strtosfix32_r(struct _clib4 *, const char *__str, char **__endptr);
extern int64_t strtosfix64(const char *__str, char **__endptr);
extern int64_t _strtosfix64_r(struct _clib4 *, const char *__str, char **__endptr);

extern uint16_t strtoufix16(const char *__str, char **__endptr);
extern uint16_t _strtoufix16_r(struct _clib4 *, const char *__str, char **__endptr);
extern uint32_t strtoufix32(const char *__str, char **__endptr);
extern uint32_t _strtoufix32_r(struct _clib4 *, const char *__str, char **__endptr);
extern uint64_t strtoufix64(const char *__str, char **__endptr);
extern uint64_t _strtoufix64_r(struct _clib4 *, const char *__str, char **__endptr);
#endif

__END_DECLS

#endif /* _STDLIB_H */
