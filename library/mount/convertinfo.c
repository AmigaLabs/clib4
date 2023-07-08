/*
 * $Id: mount_convertinfo.c,v 1.7 2008-04-16 07:38:10 clib2devs Exp $
*/

#ifndef _MOUNT_HEADERS_H
#include "mount_headers.h"
#endif /* _MOUNT_HEADERS_H */

#ifndef _POSIX_HEADERS_H
#include "posix_headers.h"
#endif /* _POSIX_HEADERS_H */

#include <limits.h>
#include <string.h>

#ifndef ID_BUSY_DISK
#define ID_BUSY_DISK (0x42555359L) /* 'BUSY' */
#endif /* ID_LONGNAME_DOS_DISK */

#ifndef ID_LONGNAME_DOS_DISK
#define ID_LONGNAME_DOS_DISK (0x444F5306L) /* 'DOS\6' */
#endif /* ID_LONGNAME_DOS_DISK */

#ifndef ID_LONGNAME_FFS_DISK
#define ID_LONGNAME_FFS_DISK (0x444F5307L) /* 'DOS\7' */
#endif /* ID_LONGNAME_FFS_DISK */

void
__convert_info_to_statfs(struct InfoData *id, struct statfs *f) {
    LONG num_blocks, num_blocks_used, bytes_per_block;

    ENTER();

    assert(id != NULL && f != NULL);

    SHOWVALUE(id->id_DiskState);
    SHOWVALUE(id->id_NumBlocks);
    SHOWVALUE(id->id_NumBlocksUsed);
    SHOWVALUE(id->id_BytesPerBlock);
    SHOWVALUE(id->id_DiskType);
    SHOWVALUE(id->id_VolumeNode);
    SHOWVALUE(id->id_InUse);

    memset(f, 0, sizeof(*f));

    if (id->id_NumBlocks > 0) {
        num_blocks = id->id_NumBlocks;
        num_blocks_used = id->id_NumBlocksUsed;
    } else {
        num_blocks = 1;
        num_blocks_used = 1;
    }

    if (id->id_BytesPerBlock > 0)
        bytes_per_block = id->id_BytesPerBlock;
    else
        bytes_per_block = 512;

    f->f_bsize = bytes_per_block;
    f->f_blocks = num_blocks;
    f->f_bfree = num_blocks - num_blocks_used;
    f->f_iosize = f->f_bsize;
    f->f_bavail = f->f_bfree;
    f->f_ffree = LONG_MAX;
    f->f_flags = MNT_NOATIME | MNT_SYMPERM | MNT_LOCAL;

    if (id->id_DiskState != ID_DISKSTATE_VALIDATED)
        SET_FLAG(f->f_flags, MNT_RDONLY);

    switch (id->id_DiskType) {
        case ID_NO_DISK_PRESENT:
            strcpy(f->f_fstypename, "None");
            break;

        case ID_UNREADABLE_DISK:
            strcpy(f->f_fstypename, "NDOS");
            break;

        case ID_BUSY_DISK:
            strcpy(f->f_fstypename, "Busy");
            break;

        case ID_DOS_DISK:
            strcpy(f->f_fstypename, "DOS\\0");
            break;

        case ID_FFS_DISK:
            strcpy(f->f_fstypename, "DOS\\1");
            break;

        case ID_INTER_DOS_DISK:
            strcpy(f->f_fstypename, "DOS\\2");
            break;

        case ID_INTER_FFS_DISK:
            strcpy(f->f_fstypename, "DOS\\3");
            break;

        case ID_FASTDIR_DOS_DISK:
            strcpy(f->f_fstypename, "DOS\\4");
            break;

        case ID_FASTDIR_FFS_DISK:
            strcpy(f->f_fstypename, "DOS\\5");
            break;

        case ID_LONGNAME_DOS_DISK:
            strcpy(f->f_fstypename, "DOS\\6");
            break;

        case ID_LONGNAME_FFS_DISK:
            strcpy(f->f_fstypename, "DOS\\7");
            break;

        case 0x53465300:
            strcpy(f->f_fstypename, "SFS\\0");
            break;

        case ID_NOT_REALLY_DOS:
            strcpy(f->f_fstypename, "NDOS");
            break;

        default:
            strcpy(f->f_fstypename, "Unknown");
            break;
    }

    SHOWVALUE(f->f_bsize);
    SHOWVALUE(f->f_blocks);
    SHOWVALUE(f->f_bfree);
    SHOWVALUE(f->f_iosize);
    SHOWVALUE(f->f_bavail);
    SHOWVALUE(f->f_ffree);
    SHOWVALUE(f->f_flags);

    LEAVE();
}

void
__convert_info_to_statvfs(struct InfoData *id, struct statvfs *f) {

    ENTER();

    assert(id != NULL && f != NULL);

    SHOWVALUE(id->id_DiskState);
    SHOWVALUE(id->id_NumBlocks);
    SHOWVALUE(id->id_NumBlocksUsed);
    SHOWVALUE(id->id_BytesPerBlock);
    SHOWVALUE(id->id_DiskType);
    SHOWVALUE(id->id_VolumeNode);
    SHOWVALUE(id->id_InUse);

    memset(f, 0, sizeof(*f));

    f->f_bsize = id->id_BytesPerBlock;
    f->f_blocks = id->id_NumBlocks ;
    f->f_bfree = id->id_NumBlocks - id->id_NumBlocksUsed;
    f->f_bavail = f->f_bfree;
    f->f_frsize = f->f_bsize;
    f->f_ffree = LONG_MAX;
    f->f_fsid = id->id_VolumeNode;
    f->f_flag = 0;

    if (id->id_DiskState != ID_DISKSTATE_VALIDATED)
        SET_FLAG(f->f_flag, ST_RDONLY);

    SHOWVALUE(f->f_bsize);
    SHOWVALUE(f->f_blocks);
    SHOWVALUE(f->f_bfree);
    SHOWVALUE(f->f_bavail);
    SHOWVALUE(f->f_frsize);
    SHOWVALUE(f->f_files);
    SHOWVALUE(f->f_ffree);
    SHOWVALUE(f->f_favail);
    SHOWVALUE(f->f_fsid);
    SHOWVALUE(f->f_flag);

    LEAVE();
}