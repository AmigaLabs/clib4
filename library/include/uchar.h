#ifndef _UCHAR_H
#define _UCHAR_H

#include <features.h>
#include <stddef.h>
#include <wchar.h>

__BEGIN_DECLS

#if !defined(__cplusplus) || (__cplusplus < 201103L)
typedef unsigned short char16_t;
typedef unsigned char32_t;
#endif

extern size_t c16rtomb(char *__restrict, char16_t, mbstate_t *__restrict);
extern size_t mbrtoc16(char16_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);

extern size_t c32rtomb(char *__restrict, char32_t, mbstate_t *__restrict);
extern size_t mbrtoc32(char32_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);

__END_DECLS

#endif
