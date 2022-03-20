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

/****************************************************************************/

extern int mblen(const char *s, size_t n);
extern int mbtowc(wchar_t *pwc, const char *s, size_t n);
extern int _mbtowc(wchar_t *pwc, const char *s, size_t n, _mbstate_t *st);

extern int wctomb(char *s, wchar_t wchar);
extern int _wctomb_r(char *s, wchar_t wchar, mbstate_t *state);
extern size_t mbstowcs(wchar_t *ws, const char *s, size_t wn);
extern size_t wcstombs(char *s, const wchar_t *pwcs, size_t n);

/****************************************************************************/

extern void *malloc(size_t size);
extern void *calloc(size_t num_elements, size_t element_size);
extern void free(void *ptr);
extern void *realloc(void *ptr, size_t size);
extern void *valloc(size_t size);
extern void *aligned_alloc(size_t alignment, size_t size);

/****************************************************************************/

extern int abs(int x);
extern long labs(long x);
extern div_t div(int n, int d);
extern ldiv_t ldiv(long n, long d);

/****************************************************************************/

extern int rand(void);
extern long random(void);
extern void srand(unsigned int seed);
extern void srandom(unsigned int seed);
extern double erand48 (unsigned short subi[3]);
extern long jrand48 (unsigned short subi[3]);
extern long nrand48 (unsigned short subi[3]);

extern char *initstate(unsigned int seed, char *state, size_t size);
extern char *setstate(const char *state);
/****************************************************************************/

extern int system(const char *command);

/****************************************************************************/

extern void exit(int status);
extern void abort(void);

/****************************************************************************/

extern int atexit(void (*)(void));

/****************************************************************************/

extern char *getenv(const char *name);
extern char *secure_getenv(const char *name);

/****************************************************************************/

extern void *bsearch(const void *key, const void *base, size_t count, size_t size,
					 int (*compare)(const void *key, const void *value));
extern void qsort(void *base, size_t count, size_t size,
				  int (*compare)(const void *element1, const void *element2));

/****************************************************************************/

extern double strtod(const char *str, char **ptr);
extern long strtol(const char *str, char **ptr, int base);
extern unsigned long strtoul(const char *str, char **ptr, int base);
extern quad_t strtoq(const char *nptr, char **endptr, register int base);

/****************************************************************************/

extern double atof(const char *str);
extern int atoi(const char *str);
extern long atol(const char *str);

extern char *itoa(int value, char *buffer, int base);
extern char *lltoa(int64_t ll, char *buffer, int radix);

extern void _exit(int status);
extern int rand_r(unsigned int *seed);

/****************************************************************************/

/*
 * If you are using the slab allocator and need to quickly release the
 * memory of all slabs which are currently unused, you can call the
 * following function to do so.
 *
 * Please note that this function works within the context of the memory
 * allocation system and is not safe to call from interrupt code. It may
 * break a Forbid() or Disable() condition.
 */
extern void __free_unused_slabs(void);

/****************************************************************************/

/*
 * You can accelerate the reuse of currently unused slabs by calling
 * the __decay_unused_slabs() function. Each call decrements the decay
 * counter until it reaches 0, at which point an unused slab can be
 * reused instead of allocating a new slab. Also, at 0 unused slabs
 * will be freed by the allocator.
 *
 * Please note that this function works within the context of the memory
 * allocation system and is not safe to call from interrupt code. It may
 * break a Forbid() or Disable() condition.
 */
extern void __decay_unused_slabs(void);

/****************************************************************************/

/*
 * You can obtain runtime statistics about the slab allocator by
 * invoking the __get_slab_usage() function which in turn invokes
 * your callback function for each single slab currently in play.
 *
 * Your callback function must return 0 if it wants to be called again,
 * for the next slab, or return -1 to stop. Note that your callback
 * function may not be called if the slab allocator is currently
 * not operational.
 *
 * Please note that this function works within the context of the memory
 * allocation system and is not safe to call from interrupt code. It may
 * break a Forbid() or Disable() condition.
 */

/****************************************************************************/

/* This is what your callback function will see when it is invoked. */
struct __slab_usage_information
{
	/* The size of all slabs, in bytes. */
	size_t sui_slab_size;

	/* Number of allocations which are not managed by slabs, but
	 * are handled separate.
	 */
	size_t sui_num_single_allocations;

	/* Total number of bytes allocated for memory not managed
	 * by slabs. This includes the management overhead for
	 * each allocation.
	 */
	size_t sui_total_single_allocation_size;

	/* Number of slabs currently in play. This can be 0. */
	size_t sui_num_slabs;

	/* Number of currently unused slabs which contain no data. */
	size_t sui_num_empty_slabs;

	/* Number of slabs in use which are completely filled with data. */
	size_t sui_num_full_slabs;

	/* Total number of bytes allocated for all slabs. */
	size_t sui_total_slab_allocation_size;

	/*
	 * The following data is updated for each slab which
	 * your callback function sees.
	 */

	/* Index number of the slab being reported (0 = no slabs are in use). */
	int sui_slab_index;

	/* How large are the memory chunks managed by this slab? */
	size_t sui_chunk_size;

	/* How many memory chunks fit into this slab? */
	size_t sui_num_chunks;

	/* How many memory chunks in this slab are being used? */
	size_t sui_num_chunks_used;

	/* How many time was this slab reused without reinitializing
	 * it all over again from scratch?
	 */
	size_t sui_num_reused;
};

/****************************************************************************/

typedef int (*__slab_usage_callback)(const struct __slab_usage_information *sui);

/****************************************************************************/

void __get_slab_usage(__slab_usage_callback callback);

/****************************************************************************/

/*
 * You can obtain runtime statistics about the memory allocations
 * which the slab allocator did not fit into slabs. This works
 * just like __get_slab_usage() in that the callback function
 * you provide will be called for each single allocation that
 * is not part of a slab.
 *
 * Your callback function must return 0 if it wants to be called again,
 * for the next slab, or return -1 to stop. Note that your callback
 * function may not be called if the slab allocator did not
 * allocate memory outside of slabs.
 *
 * Please note that this function works within the context of the memory
 * allocation system and is not safe to call from interrupt code. It may
 * break a Forbid() or Disable() condition.
 */

/* This is what your callback function will see when it is invoked. */
struct __slab_allocation_information
{
	/* Number of allocations which are not managed by slabs, but
	 * are handled separate.
	 */
	size_t sai_num_single_allocations;

	/* Total number of bytes allocated for memory not managed
	 * by slabs. This includes the management overhead for
	 * each allocation.
	 */
	size_t sai_total_single_allocation_size;

	/*
	 * The following data is updated for each slab which
	 * your callback function sees.
	 */

	/* Index number of the allocation being reported (0 = no allocations
	 * outside of slabs are in use).
	 */
	int sai_allocation_index;

	/* Size of this allocation, as requested by the program which
	 * called malloc(), realloc() or alloca().
	 */
	size_t sai_allocation_size;

	/* Total size of this allocation, including management data
	 * structure overhead.
	 */
	size_t sai_total_allocation_size;
};

/****************************************************************************/

typedef int (*__slab_allocation_callback)(const struct __slab_allocation_information *sui);

/****************************************************************************/

void __get_slab_allocations(__slab_allocation_callback callback);

/****************************************************************************/

/*
 * You can obtain information about the memory managed by the slab allocator,
 * as well as additional information about the slab allocator's performance
 * in JSON format. This format can be used for more detailed analysis.
 *
 * You supply a function which will be called for each line of the JSON
 * data produced. You can store this data in a file, or in the clipboard,
 * for later use. Your function must return 0 if it wants to be called
 * again, or return -1 if it wants to stop (e.g. if an error occured
 * when writing the JSON data to disk). The same "user_data" pointer which
 * you pass to __get_slab_stats() will be passed to your callback function.
 *
 * Please note that this function works within the context of the memory
 * allocation system and is not safe to call from interrupt code. It may
 * break a Forbid() or Disable() condition.
 */

typedef int (*__slab_status_callback)(void *user_data, const char *line, size_t line_length);

/****************************************************************************/

extern void __get_slab_stats(void *user_data, __slab_status_callback callback);

/****************************************************************************/

extern char *mktemp(char *name_template);
extern int mkstemp(char *name_template);
extern int putenv(const char *string);
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

__END_DECLS

#endif /* _STDLIB_H */
