/*
 * $Id: clib4_io.h,v 1.5 2023-07-04 14:39:23 clib4devs Exp $
*/

#ifndef	__CLIB4_IO_H__
#define	__CLIB4_IO_H__

#include <features.h>
#include <dos.h>

#include <sys/types.h>

__BEGIN_DECLS

/* Operations that can be performed by the file action function. */
enum file_action_t
{
	file_action_read,
	file_action_write,
	file_action_seek,
	file_action_close,
	file_action_set_blocking,
	file_action_set_async,
	file_action_examine
};

/****************************************************************************/

/* A message sent to a file action function. */
struct file_action_message
{
	enum file_action_t		fam_Action;		/* What to do */
	char *					fam_Data;		/* Where to read/write the data */
	int						fam_Size;		/* How much data to write */

    int64_t				    fam_Offset;		/* The seek offset */
	int						fam_Mode;		/* The seek mode */

	int						fam_Arg;		/* Action parameters, e.g. whether or not
											   this file should be set non-blocking or
											   use asynchronous I/O */

	struct ExamineData *	fam_FileInfo;	/* File information to be filled in */
	struct MsgPort *		fam_FileSystem;	/* File system pointer to be filled in */

	int						fam_Error;		/* Error code, if any... */
    int                     fam_DOSMode;    /* Canonical or Raw mode  */
};

/****************************************************************************/

/* A forward declaration to make the following typedef work. */
struct _fd;

/****************************************************************************/

/* The file action function for unbuffered files. */
typedef int64_t (*_file_action_fd_t)(struct _fd * _fd,struct file_action_message * fam);

/****************************************************************************/

/* A low level unbuffered file or socket. */
struct _fd
{
	int					fd_Version;		/* Version number of this definition
										   of the '_fd' data structure (see
										   below). */
	_file_action_fd_t	fd_Action;		/* Function to invoke to perform actions */
	void *				fd_UserData;	/* To be used by custom file action
										   functions */
	ULONG				fd_Flags;		/* File properties */

	union
	{
		BPTR			fdu_File;		/* A dos.library file handle */
		LONG			fdu_Socket;		/* A socket identifier */
	} fdu_Default;
};

/****************************************************************************/

/* The version of the '_fd' data structure, covering the fields 'fd_Version'
   through 'fdu_Default' bears version number 1. */

/****************************************************************************/

/* Sneaky preprocessor tricks to make access to the file/socket IDs
   work smoothly. */
#define fd_DefaultFile	fdu_Default.fdu_File
#define fd_File			fdu_Default.fdu_File
#define fd_Socket		fdu_Default.fdu_Socket

/****************************************************************************/

/* Flag bits that can be set in _fd->fd_Flags; not all of these may be in
   use, or can be changed. */
#define FDF_READ			(1UL<<0)	/* Data can be read from this file */
#define FDF_WRITE			(1UL<<1)	/* Data can be written to this file */
#define FDF_APPEND			(1UL<<2)	/* Before any data is written to it,
										   the file position must be set to the
										   end of the file */
#define FDF_NO_CLOSE		(1UL<<3)	/* Never close this file */
#define FDF_NON_BLOCKING	(1UL<<4)	/* File was switched into non-blocking
										   mode (console streams only) */
#define FDF_IS_SOCKET		(1UL<<5)	/* This is not a disk file but a socket */
#define FDF_IS_LOCKED		(1UL<<6)	/* This file has an advisory record lock set */
#define FDF_IN_USE			(1UL<<7)	/* This file is in use */
#define FDF_CREATED			(1UL<<8)	/* This file was newly created and may need
										   to have its protection bits updated after
										   it has been closed */
#define FDF_CACHE_POSITION	(1UL<<9)	/* Cache the file position. */
#define FDF_ASYNC_IO		(1UL<<10)	/* File was switched into asynchronous I/O
										   mode (sockets only). */
#define FDF_IS_INTERACTIVE	(1UL<<11)	/* File is attached to a console window or
										   something like it. */
#define FDF_STDIO			(1UL<<12)	/* File is to be attached to one of the
										   standard input/output/error streams. */
#define FDF_TERMIOS			(1UL<<13)	/* File is under termios control.
										   FDF_IS_INTERACTIVE should also be set. */
#define FDF_POLL			(1UL<<14)	/* File is under poll control. */
#define FDF_IS_DIRECTORY	(1UL<<15)	/* This is not a disk file but a directory */
#define FDF_PATH_ONLY   	(1UL<<16)	/* Allowed operation on file are only statistics one */
#define FDF_STDIN_AS_SOCKET	(1UL<<17)	/* STDIN is used in a socket stream */
#define FDF_STDIN_READ_MARK	(1UL<<18)	/* STDIN was marked as first read */
#define FDF_PIPE	        (1UL<<19)	/* FD is a PIPE */
#define FDF_IS_SERIAL       (1UL<<20)   /* FD is using serial hook */
#define FDF_CLOEXEC         (1UL<<21)   /* Close on child */

/****************************************************************************/

/* Obtain a pointer to the _fd data structure associated with a file
   descriptor number. Note that this does not perform any locking, which
   means that you have to be absolutely certain that the file will not be
   closed while you are still looking at it. This function can return
   NULL if the file descriptor you provided is not valid. */
extern struct _fd * __get_fd(struct _clib4 *__clib4, int file_descriptor);

/* Replaces the action callback function and (optionally) returns the old
   function pointer; returns 0 for success and -1 for failure if you
   provided an invalid file descriptor. This function performs proper locking
   and is thus safe to use in a thread-safe environment. */
extern int __change_fd_action(int file_descriptor,_file_action_fd_t new_action,_file_action_fd_t * old_action_ptr);

/* Replaces the user data pointer and (optionally) returns the old user
   data pointer; returns 0 for success and -1 for failure if you
   provided an invalid file descriptor. This function performs proper locking
   and is thus safe to use in a thread-safe environment. */
extern int __change_fd_user_data(int file_descriptor,void * new_user_data,void ** old_user_data_ptr);

__END_DECLS

#endif /* __CLIB4_IO_H__ */
