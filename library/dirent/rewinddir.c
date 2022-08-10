/*
 * $Id: dirent_rewinddir.c,v 1.5 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

void
rewinddir(DIR *directory_pointer) {
    ENTER();

    SHOWPOINTER(directory_pointer);

    __check_abort();

    if (directory_pointer != NULL) {
        struct DirectoryHandle *dh;

        dh = (struct DirectoryHandle *) directory_pointer;

        dh->dh_Position = 0;

        if (dh->dh_ScanVolumeList) {
            SHOWMSG("returning to first volume");

            dh->dh_VolumeNode = NULL;
        } else {
            SHOWMSG("calling Examine() again");
            dh->dh_FileInfo = ExamineObjectTags(EX_FileLockInput, dh->dh_DirLock, TAG_DONE);
            if (dh->dh_FileInfo == NULL) {
                SHOWMSG("ouch. that didn't work");
                __set_errno(__translate_io_error_to_errno(IoErr()));
            }
        }
    }

    LEAVE();
}
