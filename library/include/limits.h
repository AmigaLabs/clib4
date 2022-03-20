/*
 * $Id: limits.h,v 1.12 2010-08-20 15:33:36 clib2devs Exp $
*/

#ifndef _LIMITS_H
#define _LIMITS_H

#include <features.h>

#define CHAR_BIT 8

/****************************************************************************/

#define SCHAR_MIN -128
#define SCHAR_MAX 127
#define UCHAR_MAX 255

/****************************************************************************/

/*
 * The following defines the range a 'char' can cover by checking a
 * preprocessor symbol; we support both SAS/C and GCC here.
 */

#if (defined(__GNUC__) && defined(__CHAR_UNSIGNED__))

#define CHAR_MIN 0
#define CHAR_MAX 255

#else

#define CHAR_MIN -128
#define CHAR_MAX 127

#endif /* (__GNUC__ && __CHAR_UNSIGNED) */

/****************************************************************************/

#define SHRT_MIN	-32768
#define SHRT_MAX	32767
#define USHRT_MAX	65535

/****************************************************************************/

#define INT_MIN		(-2147483647 - 1)
#define INT_MAX		2147483647
#define UINT_MAX	4294967295U

/****************************************************************************/

#define LONG_MIN	(-2147483647L - 1)
#define LONG_MAX	2147483647L
#define ULONG_MAX	4294967295UL

#define LLONG_MIN	(-0x7fffffffffffffffLL-1)
#define LLONG_MAX	0x7fffffffffffffffLL
#define ULLONG_MAX	0xffffffffffffffffULL


#define MB_LEN_MAX 1

#define NL_ARGMAX 127

#define SSIZE_MAX LONG_MAX

/****************************************************************************/

#define PATH_MAX 1024

/****************************************************************************/

#define WORD_BIT 32
#define LONG_BIT 32	/* For now, at least. */

/****************************************************************************/

/*
 * POSIX, POSIX... These are minimum maximum values. I.e. the maximum must be
 * at least this large. I am not sure all of these make much sense on the Amiga
 * (yet), but here they are anyway.
 *
 * Actually, these symbols should be defined only if the preprocessor symbol
 * _POSIX_SOURCE is defined. Other symbols which should be defined, too, are
 * ARG_MAX, CHILD_MAX, LINK_MAX, MAX_CANON, MAX_INPUT, NAME_MAX, NGROUPS_MAX,
 * OPEN_MAX, PATH_MAX, PIPE_BUF, SSIZE_MAX, STREAM_MAX and TZNAME_MAX.
 */
#ifdef _POSIX_SOURCE
#define _POSIX_AIO_LISTIO_MAX		2
#define _POSIX_AIO_MAX				1
#define _POSIX_ARG_MAX				4096
#define _POSIX_CHILD_MAX			6
#define _POSIX_DELAYTIMER_MAX		32
#define _POSIX_LINK_MAX				8
#define _POSIX_LOGIN_NAME_MAX		9
#define _POSIX_MAX_CANON			255
#define _POSIX_MAX_INPUT			255
#define _POSIX_MQ_OPEN_MAX			8
#define _POSIX_MQ_PRIO_MAX			32
#define _POSIX_NAME_MAX				14
#define _POSIX_NGROUPS_MAX			0
#define _POSIX_OPEN_MAX				16
#define _POSIX_PATH_MAX				255
#define _POSIX_PIPE_BUF				512
#define _POSIX_RTSIG_MAX			8
#define _POSIX_SEM_NSEMS_MAX		256
#define _POSIX_SEM_VALUE_MAX		32767
#define _POSIX_SIGQUEUE_MAX		32
#define _POSIX_SSIZE_MAX			32767
#define _POSIX_STREAM_MAX			8
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS	4
#define _POSIX_THREAD_KEYS_MAX	128
#define _POSIX_THREADS_MAX			64
#define _POSIX_TIMER_MAX			32
#define _POSIX_TTY_NAME_MAX		9
#define _POSIX_TZNAME_MAX			3
#define _POSIX2_BC_BASE_MAX		99
#define _POSIX2_BC_DIM_MAX			2048
#define _POSIX2_BC_SCALE_MAX		99
#define _POSIX2_BC_STRING_MAX		1000
#define _POSIX2_COLL_WEIGHTS_MAX	2
#define _POSIX2_EXPR_NEST_MAX		32
#define _POSIX2_LINE_MAX			2048
#define _POSIX2_RE_DUP_MAX			255
#define _XOPEN_IOV_MAX				16

/* And a maximum minimum value. */
#define _POSIX_CLOCKRES_MIN		20000000 /* Nanoseconds */
#endif


#endif /* _LIMITS_H */
