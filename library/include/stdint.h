/*
 * $Id: stdint.h,v 1.5 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef	_STDINT_H
#define	_STDINT_H

#ifndef	_LIMITS_H
#include <limits.h>
#endif /* _LIMITS_H */

#include <features.h>

__BEGIN_DECLS

/* Common for all supported compilers. */

typedef	signed char				int8_t;
typedef	signed short			int16_t;
typedef	signed int				int32_t;
typedef	unsigned char			uint8_t;
typedef	unsigned short			uint16_t;
typedef	unsigned int			uint32_t;

typedef	signed char				int_least8_t;
typedef	signed short			int_least16_t;
typedef	signed int				int_least32_t;
typedef	unsigned char			uint_least8_t;
typedef	unsigned short			uint_least16_t;
typedef	unsigned int			uint_least32_t;

/* This is mostly guesswork. */
typedef	signed char				int_fast8_t;
typedef	signed int				int_fast16_t;
typedef	signed int				int_fast32_t;
typedef	unsigned char			uint_fast8_t;
typedef	unsigned int			uint_fast16_t;
typedef	unsigned int			uint_fast32_t;

typedef	signed long				intptr_t;
typedef	unsigned long			uintptr_t;

#if defined(__GNUC__) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
typedef	signed long long		int64_t;
typedef	unsigned long long		uint64_t;
typedef	signed long long		int_least64_t;
typedef	unsigned long long		uint_least64_t;
typedef	signed long long		int_fast64_t;
typedef	unsigned long long		uint_fast64_t;
typedef	signed long long		intmax_t;
typedef	unsigned long long		uintmax_t;
#else
typedef	signed long 			intmax_t;
typedef	unsigned long		 	uintmax_t;
#endif /* __GNUC__ || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L) */

#define	INT8_MIN				SCHAR_MIN
#define	INT8_MAX				SCHAR_MAX
#define	UINT8_MAX				UCHAR_MAX
#define	INT16_MIN				SHRT_MIN
#define	INT16_MAX				SHRT_MAX
#define	UINT16_MAX				USHRT_MAX
#define	INT32_MIN				INT_MIN
#define	INT32_MAX				INT_MAX
#define	UINT32_MAX				UINT_MAX
#if defined(__GNUC__) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
#define	INT64_MIN				LLONG_MIN
#define	INT64_MAX				LLONG_MAX
#define	UINT64_MAX				ULLONG_MAX
#endif /* __GNUC__ || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L) */

#define	INT_LEAST8_MIN			SCHAR_MIN
#define	INT_LEAST8_MAX			SCHAR_MAX
#define	UINT_LEAST8_MAX			UCHAR_MAX
#define	INT_LEAST16_MIN			SHRT_MIN
#define	INT_LEAST16_MAX			SHRT_MAX
#define	UINT_LEAST16_MAX		USHRT_MAX
#define	INT_LEAST32_MIN			INT_MIN
#define	INT_LEAST32_MAX			INT_MAX
#define	UINT_LEAST32_MAX		UINT_MAX
#if defined(__GNUC__) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
#define	INT_LEAST64_MIN			LLONG_MIN
#define	INT_LEAST64_MAX			LLONG_MAX
#define	UINT_LEAST64_MAX		ULLONG_MAX
#endif /* __GNUC__ || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L) */

#define	INT_FAST8_MIN			SCHAR_MIN
#define	INT_FAST8_MAX			SCHAR_MAX
#define	UINT_FAST8_MAX			UCHAR_MAX
#define	INT_FAST16_MIN			INT_MIN
#define	INT_FAST16_MAX			INT_MAX
#define	UINT_FAST16_MAX			UINT_MAX
#define	INT_FAST32_MIN			INT_MIN
#define	INT_FAST32_MAX			INT_MAX
#define	UINT_FAST32_MAX			UINT_MAX
#if defined(__GNUC__) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
#define	INT_FAST64_MIN			LLONG_MIN
#define	INT_FAST64_MAX			LLONG_MAX
#define	UINT_FAST64_MAX			ULLONG_MAX
#endif /* __GNUC__ || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L) */

#define	INTPTR_MIN				LONG_MIN
#define	INTPTR_MAX				LONG_MAX
#define	UINTPTR_MAX				ULONG_MAX

#if defined(__GNUC__) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
#define	INTMAX_MIN				LLONG_MIN
#define	INTMAX_MAX				LLONG_MAX
#define	UINTMAX_MAX				ULLONG_MAX
#else
#define	INTMAX_MIN				LONG_MIN
#define	INTMAX_MAX				LONG_MAX
#define	UINTMAX_MAX				ULONG_MAX
#endif /* __GNUC__ || (__STDC_VERSION__ && __STDC_VERSION__ >= 199901L) */

/* sigatomic_t is an int. */
#define	SIG_ATOMIC_MIN			INT_MIN
#define	SIG_ATOMIC_MAX			INT_MAX

/* Maximum value of size_t */
#define	SIZE_MAX				UINT_MAX

#define INT8_C(c)  c
#define INT16_C(c) c
#define INT32_C(c) c

#define UINT8_C(c)  c
#define UINT16_C(c) c
#define UINT32_C(c) c ## U

#if UINTPTR_MAX == UINT64_MAX
#define INT64_C(c) c ## L
#define UINT64_C(c) c ## UL
#define INTMAX_C(c)  c ## L
#define UINTMAX_C(c) c ## UL
#else
#define INT64_C(c) c ## LL
#define UINT64_C(c) c ## ULL
#define INTMAX_C(c)  c ## LL
#define UINTMAX_C(c) c ## ULL
#endif

/****************************************************************************/

#define PTRDIFF_MIN	INT_MIN
#define PTRDIFF_MAX	INT_MAX

#define WCHAR_MIN	0
#define WCHAR_MAX	USHRT_MAX

#define WINT_MIN	INT_MIN
#define WINT_MAX	INT_MAX



__END_DECLS

#endif /* _STDINT_H */
