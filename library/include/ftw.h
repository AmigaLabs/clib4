/*
 * $Id: ftw.h,v 1.5 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef	_FTW_H
#define	_FTW_H

#include <sys/stat.h>

#include <features.h>

__BEGIN_DECLS

/* Values passed to the user function (argument #3). */
enum
{
	FTW_F,		/* Regular file. */
	FTW_D,		/* Directory. */
	FTW_DP,		/* Directory, already visited. (nftw() only) */
	FTW_SL,		/* Symlink. (nftw() only) */
	FTW_SLN,	/* Broken Symlink. (does not point to an existing file, nftw() only) */
	FTW_DNR,	/* Directory which can not be read (e.g. not enough permissions) */
	FTW_NS		/* Stat failed. */
};

/****************************************************************************/

/* Flags for FTW.quit */
#define	FTW_SKD		(1L<<0)	/* Skip directory. */
#define	FTW_PRUNE	(1L<<1)	/* Prune traversal. (skip up) */

/****************************************************************************/

/* Flags for nftw() */
#define	FTW_PHYS		(1L<<0)
#define	FTW_MOUNT		(1L<<1)		/* Currently unsupported. */
#define	FTW_DEPTH		(1L<<2)
#define	FTW_CHDIR		(1L<<3)
#define	FTW_ALL_FLAGS	(0x0000000f)

/****************************************************************************/

struct FTW
{
	int quit;	/* Flags passed out from the user function to ftw()/nftw() */
	int base;	/* Index of current item from start of string. e.g. 4 for "foo/bar" */
	int level;	/* Current depth. (counted from 0) */
};

/****************************************************************************/

extern int ftw(const char *path,int (*func)(const char *,const struct stat *,int),int depth);
extern int nftw(const char *path,int (*func)(const char *,const struct stat *,int,struct FTW *),int depth,int flags);

__END_DECLS

#endif /* _FTW_H */
