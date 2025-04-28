/*
 * $Id: types.h,v 1.10 2023-06-03 12:06:14 clib4devs Exp $
*/

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <features.h>

#include <stddef.h>
#include <stdint.h>

__BEGIN_DECLS

typedef char * caddr_t;
typedef unsigned int comp_t;
typedef unsigned long dev_t;
typedef unsigned int gid_t;
typedef unsigned int ino_t;
typedef unsigned int mode_t;
typedef unsigned int nlink_t;
typedef int64_t	off_t;
typedef uint64_t fsblkcnt_t;
typedef uint64_t fsfilcnt_t;
#ifdef __USE_LARGEFILE64
    typedef int64_t _off64_t;
    typedef int64_t off64_t;
    typedef int64_t _fpos64_t;
    typedef uint64_t ino64_t;
    typedef int64_t	blkcnt64_t;
    typedef uint64_t fsblkcnt64_t;
    typedef uint64_t fsfilcnt64_t;
#endif
typedef int pid_t;
typedef unsigned int rlim_t;
typedef unsigned int uid_t;

#if defined(INT_MAX) && INT_MAX == 2147483647
typedef int _ssize_t;
#else
typedef long _ssize_t;
#endif
typedef _ssize_t ssize_t;

#ifndef _BSDTYPES_DEFINED
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
#define _BSDTYPES_DEFINED
#endif

typedef	unsigned char u_int8_t;
typedef	unsigned short u_int16_t;
typedef	unsigned int u_int32_t;
typedef	unsigned long long u_int64_t;

typedef unsigned long useconds_t;
typedef long suseconds_t;

typedef int32_t blksize_t;
#ifndef __USE_FILE_OFFSET64
typedef int32_t blkcnt_t;
#else
typedef int64_t blkcnt_t;
#endif

typedef	unsigned long long u_quad_t;	/* quads */
typedef	long long	quad_t;
typedef	quad_t *	qaddr_t;

#define UQUAD_MAX       ((u_quad_t)(0-1)) /* max value for a uquad_t */
#define QUAD_MAX        ((quad_t)(UQUAD_MAX >> 1))
#define QUAD_MIN        (-QUAD_MAX-1)   /* min value for a quad_t */

/* Iconv descriptor type */
typedef void *_iconv_t;

#define _CLOCK_T_ unsigned long     /* clock() */
#define _TIME_T_ long               /* time() */
#define _CLOCKID_T_ unsigned long
#define _TIMER_T_ unsigned long

typedef _CLOCK_T_   clock_t;
typedef _TIME_T_    time_t;
typedef _CLOCKID_T_ clockid_t;

#ifndef        FD_SETSIZE
#define        FD_SETSIZE 256
#endif

typedef struct _types_fd_set {
    unsigned long fds_bits[(FD_SETSIZE + 31) / 32];
} _types_fd_set;

#define fd_set _types_fd_set

#define FD_SET(n, p)    ((p)->fds_bits[(n)/32] |= (1L << ((n) % 32)))
#define FD_CLR(n, p)    ((p)->fds_bits[(n)/32] &= ~(1L << ((n) % 32)))
#define FD_ISSET(n, p)  ((p)->fds_bits[(n)/32] & (1L << ((n) % 32)))
#define FD_ZERO(p)      (__extension__ (void)({ \
     size_t __i; \
     char *__tmp = (char *)p; \
     for (__i = 0; __i < sizeof (*(p)); ++__i) \
       *__tmp++ = 0; \
}))

__END_DECLS

#endif /* _SYS_TYPES_H */
