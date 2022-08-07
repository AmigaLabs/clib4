/*
 * $Id: stdlib.h,v 1.19 2008-04-30 14:34:03 clib2devs Exp $
*/

#ifndef _STDLIB_H
#define _STDLIB_H

#include <features.h>

#include <stddef.h>
#include <wchar.h>
#include <alloca.h>
#include <sys/types.h>

#ifndef _SYS_CLIB2_STDC_H
#include <sys/clib2_stdc.h>
#endif /* _SYS_CLIB2_STDC_H */

__BEGIN_DECLS

/* Maximum number of bytes in a multibyte character */
extern  int *__mb_cur_max(void);
#define MB_CUR_MAX (*__mb_cur_max())

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
extern int putenv(const char *string);
extern char *l64a (long x0);
extern long a64l (const char *s);
extern void setkey(const char *key);

double drand48 (void);
double erand48 (unsigned short subi[3]);
long int lrand48 (void);
long int nrand48 (unsigned short subi[3]);
long mrand48 (void);
long jrand48 (unsigned short subi[3]);
void srand48 (long);
unsigned short *seed48 (unsigned short subi[3]);
void lcong48 (unsigned short subi[7]);

#endif


extern int system(const char *command);
extern void exit(int status);
extern void abort(void);

extern int atexit(void (*)(void));
extern char *getenv(const char *name);
extern void *bsearch(const void *key, const void *base, size_t count, size_t size, int (*compare)(const void *key, const void *value));
extern void qsort(void *base, size_t count, size_t size, int (*compare)(const void *element1, const void *element2));

extern double strtod(const char *str, char **ptr);
extern long strtol(const char *str, char **ptr, int base);
extern unsigned long strtoul(const char *str, char **ptr, int base);
extern quad_t strtoq(const char *nptr, char **endptr, register int base);

extern double atof(const char *str);
extern int atoi(const char *str);
extern long atol(const char *str);

extern char *itoa(int value, char *buffer, int base);
extern char *lltoa(int64_t ll, char *buffer, int radix);

extern void _exit(int status);
extern int rand_r(unsigned int *seed);

extern char *mktemp(char *name_template);
extern int mkstemp(char *name_template);
extern int unsetenv(const char *name);
extern int setenv(const char *name, const char *value, int overwrite);

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

extern float atoff (const char *nptr);
#endif
extern void _Exit(int status);

#ifdef _GNU_SOURCE
extern char *ecvt(double x, int n, int *dp, int *sign);
extern char *fcvt(double x, int n, int *dp, int *sign);
extern char *gcvt(double x, int n, char *b);
extern char *secure_getenv(const char *name);
#endif

__END_DECLS

#endif /* _STDLIB_H */
