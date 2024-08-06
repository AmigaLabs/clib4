/*
 * $Id: string_headers.h,v 1.7 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#define _STRING_HEADERS_H

/****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <locale.h>
#include <errno.h>
#include <ctype.h>
#include <dos.h>

/****************************************************************************/

#ifndef _STDLIB_LOCALEBASE_H
#include "stdlib_localebase.h"
#endif /* _STDLIB_LOCALEBASE_H */

#ifndef _STDLIB_UTILITYBASE_H
#include "stdlib_utilitybase.h"
#endif /* _STDLIB_UTILITYBASE_H */

/****************************************************************************/

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#ifndef _DEBUG_H
#include "debug.h"
#endif /* _DEBUG_H */

/* Address is neither aligned to a word or long word boundary. */
#define IS_UNALIGNED(a) ((((unsigned long)(a)) & 1) != 0)

/* Address is aligned to a word boundary, but not to a long
   word boundary. */
#define IS_SHORT_ALIGNED(a) ((((unsigned long)(a)) & 3) == 2)

/* Address is aligned to a long word boundary. For an 68030 and beyond the
   alignment does not matter. */
#define IS_LONG_ALIGNED(a) ((((unsigned long)(a)) & 3) == 0)

/****************************************************************************/

/* 4x0 specific asm functions */
extern void   *__memchr440(const void *ptr, int val, size_t len);
extern int     __memcmp440(const void *m1, const void *m2, size_t len);
extern void   *__memcpy440(void *dst, const void *src, size_t len);
extern void   *__memmove440(void *dest, const void *src, size_t len);
extern int     __strcmp440(const char *s1, const char *s2);
extern char   *__strcpy440(char *dest, const char *src);
extern size_t  __strlen440(const char *s);
extern int     __strncmp440(const char *s1, const char *s2, size_t n);
extern char   *__strrchr440(const char *s, int c);
extern char   *__strchr440(const char *s, int c);

/* SPE e500 specific functions */
extern int     __memcmp_e500(const char *m1, const char *m2, size_t len);
extern int     __strcmp_e500(const char *s1, const char *s2);
extern char   *__strcpy_e500(char *dest, const char *src);
extern size_t  __strlen_e500(const char *s);

/* Altivec specific asm functions */
extern void   *vec_memchr(const void *ptr, int val, size_t len);
extern int     vec_memcmp(const void *m1, const void *m2, size_t len);
extern void   *_vec_memcpy(void *dst, const void *src, size_t len);
extern void   *vec_memmove(void *dst, const void *src, size_t len);
extern void    vec_bcopy(const void *src, void *dest, size_t len);
extern void   *_vec_memset(void *ptr, int val, size_t len);
extern void    vec_bzero(void *m, size_t len);
extern char   *vec_strchr(const char *s, int c);
extern char   *vec_strcpy(char *dest, const char *src);

/* Generic PPC functions */
extern void    bcopy_g3(const void *src, void *dest, size_t len);
extern char   *__strcpy_ppc(char *dest, const char *src);
extern size_t  __strlen_ppc(const char *s);
extern char   *__strcat_ppc(char *dest, const char *src);
extern char   *__strchr_ppc(const char *s, int c);

/* Other optimized functions */
extern char   *glibc_strchr(const char *s, int c);

#endif /* _STRING_HEADERS_H */
