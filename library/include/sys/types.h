/*
 * $Id: types.h,v 1.9 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <features.h>

#include <time.h>
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
typedef signed int ssize_t;
typedef unsigned int uid_t;

#ifndef _BSDTYPES_DEFINED
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
#define _BSDTYPES_DEFINED
#endif

typedef unsigned long useconds_t;
typedef long suseconds_t;

typedef int32_t blksize_t;

typedef	unsigned long long u_quad_t;	/* quads */
typedef	long long	quad_t;
typedef	quad_t *	qaddr_t;

#define UQUAD_MAX       ((u_quad_t)(0-1)) /* max value for a uquad_t */
#define QUAD_MAX        ((quad_t)(UQUAD_MAX >> 1))
#define QUAD_MIN        (-QUAD_MAX-1)   /* min value for a quad_t */

/* Iconv descriptor type */
typedef void *_iconv_t;

__END_DECLS

#endif /* _SYS_TYPES_H */
