/*
 * $Id: mount.h,v 1.7 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif /* _SYS_TYPES_H */

#include <features.h>

__BEGIN_DECLS

typedef struct { long val[2]; } fsid_t; /* file system id type */
 
/****************************************************************************/

#define MFSNAMELEN	16 /* length of fs type name, including nul */
#define MNAMELEN	90 /* length of buffer for returned name */

/****************************************************************************/

struct statfs
{
	long	f_bsize;					/* fundamental file system block size */
	long	f_iosize;					/* optimal transfer block size */
	long	f_blocks;					/* total data blocks in file system */
	long	f_bfree;					/* free blocks in fs */
	long	f_bavail;					/* free blocks avail to non-superuser */
	long	f_files;					/* total file nodes in file system */
	long	f_ffree;					/* free file nodes in fs */
	fsid_t	f_fsid;						/* file system id */
	uid_t	f_owner;					/* user that mounted the file system */
	long	f_flags;					/* copy of mount flags */
	long	f_syncwrites;				/* count of sync writes since mount */
	long	f_asyncwrites;				/* count of async writes since mount */
	char	f_fstypename[MFSNAMELEN];	/* fs type name */
	char	f_mntonname[MNAMELEN];		/* directory on which mounted */
	char	f_mntfromname[MNAMELEN];	/* mounted file system */
};

/****************************************************************************/

#define MNT_RDONLY		(1<<0) /* The filesystem is mounted read-only */
#define MNT_NOEXEC		0
#define MNT_NOSUID		0
#define MNT_NODEV		0
#define MNT_SYNCHRONOUS	0
#define MNT_ASYNC		0
#define MNT_UNION		0
#define MNT_NOCOREDUMP	0
#define MNT_NOATIME		(1<<1) /* Never update access times */
#define MNT_SYMPERM		(1<<2) /* Recognize symbolic link permission */
#define MNT_NODEVMTIME	0
#define MNT_SOFTDEP		0
#define MNT_LOCAL		(1<<3) /* The filesystem resides locally */
#define MNT_QUOTA		0
#define MNT_ROOTFS		0
#define MNT_EXRDONLY	0
#define MNT_EXPORTED	0
#define MNT_DEFEXPORTED	0
#define MNT_EXPORTANON	0
#define MNT_EXKERB		0
#define MNT_EXNORESPORT	0
#define MNT_EXPUBLIC	0

/****************************************************************************/

extern int statfs(const char *path, struct statfs *buf);
extern int fstatfs(int fd, struct statfs *buf);

__END_DECLS

#endif /* _SYS_MOUNT_H */
