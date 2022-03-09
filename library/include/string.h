/*
 * $Id: string.h,v 1.12 2006-09-17 17:36:42 obarthel Exp $
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
 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 */

#ifndef _STRING_H
#define _STRING_H

/****************************************************************************/

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
extern char *strcasestr(const char *haystack, const char *needle);

/****************************************************************************/

extern int strcoll(const char *s1, const char *s2);
extern size_t strxfrm(char *dest, const char *src, size_t len);

/****************************************************************************/

extern void *memmove(void *dest, const void * src, size_t len);
extern void *memchr(const void * ptr, int val, size_t len);
extern void *memrchr(const void *s, int c, size_t n);

/* This is ugly: GCC 2.95.x assumes that 'unsigned long' is used in the built-in
   memcmp/memcpy/memset functions instead of 'size_t'. This can produce warnings
   where none are necessary. */
#if defined(__GNUC__) && (__GNUC__ < 3)
extern int memcmp(const void *ptr1, const void *ptr2, unsigned long len);
extern void *memcpy(void *dest, const void *src, unsigned long len);
extern void *memset(void *ptr, int val, unsigned long len);
#else
extern int memcmp(const void *ptr1, const void *ptr2, size_t len);
extern void *memcpy(void *dest, const void *src, size_t len);
extern void *memset(void *ptr, int val, size_t len);
#endif /* __GNUC__ && __GNUC__ < 3 */

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
extern int bcmp(const void *a,const void *b,size_t len);

/****************************************************************************/

extern size_t strlcpy(char *dst, const char *src, size_t siz);
extern size_t strlcat(char *dst, const char *src, size_t siz);

/****************************************************************************/

extern char * strtok_r(char *str, const char *separator_set,char ** state_ptr);

/****************************************************************************/
#ifndef __STRICT_ANSI__
extern void *memccpy(void *dst0, const void *src0, int endchar0, size_t len0);
extern void *mempcpy(void *dst0, const void *src0, size_t len0);
extern char *index(const char *s, int c);
extern char *rindex(const char *s, int c);
#endif

__END_DECLS

#endif /* _STRING_H */
