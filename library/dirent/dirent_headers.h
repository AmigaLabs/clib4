/*
 * $Id: dirent_headers.h,v 1.9 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#define _DIRENT_HEADERS_H

/****************************************************************************/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

/****************************************************************************/

struct DirectoryHandle
{
	struct MinNode			dh_MinNode;
	BPTR					dh_DirLock;
	struct ExamineData *	dh_FileInfo;
	APTR 					dh_Context; 
	struct dirent			dh_DirectoryEntry;

	int						dh_Position;
	struct Node *			dh_VolumeNode;
	struct MinList			dh_VolumeList;
	BOOL					dh_ScanVolumeList;
};

extern struct MinList NOCOMMON __directory_list;

extern void __dirent_lock(void);
extern void __dirent_unlock(void);

#endif /* _DIRENT_HEADERS_H */
