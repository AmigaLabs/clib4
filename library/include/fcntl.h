/*
 * $Id: fcntl.h,v 1.9 2022-08-11 12:06:14 clib2devs Exp $
*/

#ifndef _FCNTL_H
#define _FCNTL_H

#include <features.h>
#include <sys/types.h>
#include <stddef.h>

__BEGIN_DECLS

#define O_RDONLY	0
#define O_WRONLY	1
#define O_RDWR		2

#define O_APPEND	(1<<2)
#define O_CREAT		(1<<3)
#define O_EXCL		(1<<4)
#define O_TRUNC		(1<<5)
#define O_NONBLOCK	(1<<6)
#define O_NDELAY	O_NONBLOCK
#define O_NOCTTY	(0)
#define O_ASYNC		(1<<7)
#define O_PATH      (1<<8)
#define O_DIRECTORY (1<<9)
#define O_DSYNC     (1<<10)
#define O_SYNC		(1<<11)
#define O_CLOEXEC   (0)

#define	O_ACCMODE	(O_RDONLY|O_WRONLY|O_RDWR)

/****************************************************************************/

#define F_DUPFD		0
#define F_GETFD		1
#define F_SETFD		2
#define F_GETFL		3
#define F_SETFL		4
#define F_GETOWN	5
#define F_SETOWN	6

/*
 * Advisory file segment locking data type -
 * information passed to system by user
 */
struct flock {
	short	l_type;		/* lock type: read/write, etc. */
	short	l_whence;	/* type of l_start */
	off_t	l_start;	/* starting offset */
	off_t	l_len;		/* len = 0 means until end of file */
	pid_t	l_pid;		/* lock owner */
};

#define F_GETLK		100	/* get record locking information */
#define F_SETLK		101	/* set record locking information */
#define F_SETLKW	102	/* F_SETLK; wait if blocked */

#define F_RDLCK		1	/* shared or read lock */
#define F_UNLCK		2	/* unlock */
#define F_WRLCK		3	/* exclusive or write lock */

#define FD_CLOEXEC  1   /* posix */

#define AT_FDCWD            (-100) /* openat */
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR        0x200
#define AT_SYMLINK_FOLLOW   0x400
#define AT_EACCESS          0x200

extern int open(const char *path_name, int open_flag, ... /* mode_t mode */ );
extern int openat(int fd, const char *filename, int flags, ...);
extern int creat(const char * path_name, mode_t mode);
extern int close(int file_descriptor);
extern ssize_t read(int file_descriptor, void * buffer, size_t num_bytes);
extern ssize_t write(int file_descriptor, const void * buffer, size_t num_bytes);
extern int fcntl(int file_descriptor, int cmd, ... /* int arg */ );

__END_DECLS

#endif /* _FCNTL_H */
