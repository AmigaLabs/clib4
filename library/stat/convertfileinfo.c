/*
 * $Id: stat_convertfileinfo.c,v 1.9 2006-09-20 19:46:57 clib4devs Exp $
*/

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

void __convert_file_info_to_stat(
	const struct MsgPort *file_system,
	const struct ExamineData *fib,
	struct stat *st) // TODO - Change to stat64
{
	time_t mtime;
	mode_t mode;
	ULONG flags;

	ENTER();

	assert(fib != NULL && st != NULL);

	SHOWVALUE(fib->ObjectID);
	SHOWVALUE(fib->Type);
	SHOWSTRING(fib->Name);
	SHOWVALUE(fib->Protection);
	SHOWVALUE(fib->FileSize);
	SHOWVALUE(fib->Date.ds_Days);
	SHOWVALUE(fib->Date.ds_Minute);
	SHOWVALUE(fib->Date.ds_Tick);
	SHOWSTRING(fib->Comment);
	SHOWVALUE(fib->OwnerUID);
	SHOWVALUE(fib->OwnerGID);

	memset(st, 0, sizeof(*st));

	flags = fib->Protection;

	if (EXD_IS_PIPE(fib))
		mode = S_IFIFO;
	else if (EXD_IS_SOCKET(fib))
		mode = S_IFSOCK;
	else if (EXD_IS_FILE(fib))
		mode = S_IFREG;
	else if (EXD_IS_DIRECTORY(fib))
		mode = S_IFDIR;
	else if (EXD_IS_SOFTLINK(fib))
		mode = S_IFLNK;
	else
		mode = S_IFREG; // CHeck for ST_CONSOLE and ST_NIL that should became S_IFCHR

	if (FLAG_IS_CLEAR(flags, EXDF_NO_READ))
		SET_FLAG(mode, S_IRUSR);

	if (FLAG_IS_CLEAR(flags, EXDF_NO_WRITE) && FLAG_IS_CLEAR(flags, EXDF_NO_DELETE))
		SET_FLAG(mode, S_IWUSR);

	if (FLAG_IS_CLEAR(flags, EXDF_NO_EXECUTE))
		SET_FLAG(mode, S_IXUSR);

	if (FLAG_IS_SET(flags, EXDF_GRP_READ))
		SET_FLAG(mode, S_IRGRP);

	if (FLAG_IS_SET(flags, EXDF_GRP_WRITE) && FLAG_IS_SET(flags, EXDF_GRP_DELETE))
		SET_FLAG(mode, S_IWGRP);

	if (FLAG_IS_SET(flags, EXDF_GRP_EXECUTE))
		SET_FLAG(mode, S_IXGRP);

	if (FLAG_IS_SET(flags, EXDF_OTR_READ))
		SET_FLAG(mode, S_IROTH);

	if (FLAG_IS_SET(flags, EXDF_OTR_WRITE) && FLAG_IS_SET(flags, EXDF_OTR_DELETE))
		SET_FLAG(mode, S_IWOTH);

	if (FLAG_IS_SET(flags, EXDF_OTR_EXECUTE))
		SET_FLAG(mode, S_IXOTH);

	mtime = __convert_datestamp_to_time(&fib->Date);

	if (!EXD_IS_DIRECTORY(fib))
	{
		st->st_nlink = 1;
		st->st_size = fib->FileSize;
	}
	else
	{
		st->st_nlink = 2;
	}

	st->st_ino = fib->ObjectID;
	st->st_dev = (dev_t)file_system;
	st->st_mode = mode;
	st->st_mtime = mtime;
	st->st_atime = mtime;
	st->st_ctime = mtime;
	st->st_uid = fib->OwnerUID;
	st->st_gid = fib->OwnerGID;
	st->st_blksize = 512;
	st->st_blocks = (st->st_size + st->st_blksize - 1) / st->st_blksize;

	SHOWVALUE(st->st_nlink);
	SHOWVALUE(st->st_size);
	SHOWVALUE(st->st_ino);
	SHOWVALUE(st->st_dev);
	SHOWVALUE(st->st_mode);
	SHOWVALUE(st->st_mtime);
	SHOWVALUE(st->st_atime);
	SHOWVALUE(st->st_ctime);
	SHOWVALUE(st->st_uid);
	SHOWVALUE(st->st_gid);
	SHOWVALUE(st->st_blksize);
	SHOWVALUE(st->st_blocks);

	LEAVE();
}
