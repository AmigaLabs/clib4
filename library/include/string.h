/*
 * $Id: string.h,v 1.12 2006-09-17 17:36:42 clib2devs Exp $
*/

#ifndef _STRING_H
#define _STRING_H

#ifndef _STDDEF_H
#include <stddef.h>
#endif /* _STDDEF_H */

#include <features.h>

__BEGIN_DECLS

extern char *strerror(int error_number);
extern char *strcat(char *dest, const char *src);
extern char *strncat(char *dest, const char *src, size_t n);
extern int strcmp(const char *s1, const char * s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern char *strcpy(char *dest, const char *src);
extern char *strncpy(char *dest, const char *src, size_t n);
extern size_t strnlen(const char *s, size_t maxlen);
extern size_t strlen(const char *s);
extern char *strchr(const char *s, int c);
extern char *strrchr(const char *s, int c);
extern size_t strspn(const char *s, const char *set);
extern size_t strcspn(const char *s, const char *set);
extern char *strpbrk(const char *s, const char *set);
extern char *strtok(char *str, const char *set);
extern char *strstr(const char *src, const char *sub);
extern char *strnstr(const char *src, const char *sub, size_t len);
extern char *strsep(char ** src, const char *delim);
extern char *stpcpy(char *dest, const char *src);
extern char *stpncpy(char *dest, const char *src, size_t len);

// Used for AROS compatibility
extern size_t stccpy (char * dest, const char * src, size_t n);

extern int strcoll(const char *s1, const char *s2);
extern size_t strxfrm(char *dest, const char *src, size_t len);

extern void *memmove(void *dest, const void * src, size_t len);
extern void *memchr(const void * ptr, int val, size_t len);
extern int memcmp(const void *ptr1, const void *ptr2, size_t len);
extern void *memcpy(void *dest, const void *src, size_t len);
extern void *memset(void *ptr, int val, size_t len);

#ifndef _STRINGS_H
#include <strings.h>
#endif /* _STRINGS_H */

/****************************************************************************/

extern int strerror_r(int error,char * buffer,size_t buffer_size);

/****************************************************************************/

extern char *strdup(const char *s);
extern char *strndup(const char *str, size_t max);

/****************************************************************************/

extern void bcopy(const void *from,void *to,size_t len);
extern void bzero(void *m,size_t len);
extern void explicit_bzero(void *m, size_t len);
extern int bcmp(const void *a,const void *b,size_t len);

/****************************************************************************/

extern size_t strlcpy(char *dst, const char *src, size_t siz);
extern size_t strlcat(char *dst, const char *src, size_t siz);

/****************************************************************************/

extern char *strtok_r(char *str, const char *separator_set,char ** state_ptr);
extern char *strsignal(int);

/****************************************************************************/
#ifndef __STRICT_ANSI__
extern void *memccpy(void *dst0, const void *src0, int endchar0, size_t len0);
extern char *index(const char *s, int c);
extern char *rindex(const char *s, int c);
#endif

#ifdef _GNU_SOURCE
#define	strdupa(x)	strcpy(alloca(strlen(x)+1),x)
extern int strverscmp(const char *s1, const char *s2);
extern char *strchrnul(const char *s, int c_in);
extern char *strcasestr(const char *haystack, const char *needle);
extern void *memmem(const void *h0, size_t k, const void *n0, size_t l);
extern void *memrchr(const void *s, int c, size_t n);
extern void *mempcpy(void *dst0, const void *src0, size_t len0);
#endif

__END_DECLS

#endif /* _STRING_H */
