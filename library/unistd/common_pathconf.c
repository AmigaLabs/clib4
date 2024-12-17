/*
 * $Id: unistd_common_pathconf.c,v 1.4 2008-04-16 07:38:10 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef ID_CON
#define ID_CON (0x434F4E00L) /* 'CON\0' */
#endif /* ID_CON */

#ifndef ID_RAWCON
#define ID_RAWCON (0x52415700L) /* 'RAW\0' */
#endif /* ID_RAWCON */

#ifndef ID_BUSY_DISK
#define ID_BUSY_DISK (0x42555359L) /* 'BUSY' */
#endif /* ID_LONGNAME_DOS_DISK */

#ifndef ID_LONGNAME_DOS_DISK
#define ID_LONGNAME_DOS_DISK (0x444F5306L) /* 'DOS\6' */
#endif /* ID_LONGNAME_DOS_DISK */

#ifndef ID_LONGNAME_FFS_DISK
#define ID_LONGNAME_FFS_DISK (0x444F5307L) /* 'DOS\7' */
#endif /* ID_LONGNAME_FFS_DISK */

struct fs_info {
    ULONG dos_type;
    int name_max, path_max;
    int link_max, symlink_max;
    int file_size_bits;
};

static const struct fs_info fs_info[] =
        {
                {ID_NO_DISK_PRESENT,   0,   0,    0,  0,  0},        /* No disk */
                {ID_UNREADABLE_DISK,   0,   0,    0,  0,  0},        /* BAD\0 */
                {ID_BUSY_DISK,         0,   0,    0,  0,  0},        /* BUSY - Diskchange in progress? */
                {ID_CON,               0,   0,    0,  0,  0},        /* CON\0 - Not really a disk. */
                {ID_RAWCON,            0,   0,    0,  0,  0},        /* RAW\0 - Not really a disk. */
                {ID_NOT_REALLY_DOS,    0,   0,    0,  0,  0},        /* Unreadable disk. */
                {ID_KICKSTART_DISK,    0,   0,    0,  0,  0},        /* Kickstart disk. */
                {ID_MSDOS_DISK,        8,   31,   0,  0,  30},    /* MSDOS - TODO: Check what the limits really are. */
                {ID_DOS_DISK,          30,  254,  -1, -1, 31},    /* DOS\0 = OFS */
                {ID_FFS_DISK,          30,  254,  -1, -1, 31},    /* DOS\1 = FFS */
                {ID_INTER_DOS_DISK,    30,  254,  -1, -1, 31},    /* DOS\2 = OFS INTL */
                {ID_INTER_FFS_DISK,    30,  254,  -1, -1, 31},    /* DOS\3 = FFS INTL */
                {ID_FASTDIR_DOS_DISK,  30,  254,  -1, -1, 31},    /* DOS\4 = OFS DC */
                {ID_FASTDIR_FFS_DISK,  30,  254,  -1, -1, 31},    /* DOS\5 = FFS DC */
                {ID_LONGNAME_DOS_DISK, 107, 8192, -1, -1, 31},    /* DOS\6 = OFS LONGNAMES */
                {ID_LONGNAME_FFS_DISK, 107, 8192, -1, -1, 63},    /* DOS\7 = FFS LONGNAMES */
                {0x53465300,           107, 8192, 0,  -1, 31},    /* SFS\0 = Smartfilesystem */
                {0x53465302,           107, 8192, 0,  -1, 63},    /* SFS\2 = Smartfilesystem2 */
                {0x50465300,           30,  254,  0,  0,  31},    /* PFS\0 = Professional File System */
                {0x46545854,           0,   0,    0,  0,  0},        /* FTXT - Textclip device */
        };

long
__pathconf(struct MsgPort *port, int name) {
    ULONG dos_type = 0;
    size_t fs_index = 0;
    long ret = -1;

    if (port != NULL) {
        D_S(struct InfoData,id);

        if (DoPkt(port, ACTION_IS_FILESYSTEM, 0, 0, 0, 0, 0) == DOSFALSE) /* Not a filesystem. */
        {
            SHOWMSG("Not a filesystem.");

            __set_errno(ENODEV);
            goto out;
        }

        if (DoPkt(port, ACTION_HANDLER_INFO, MKBADDR(id), 0, 0, 0, 0))    /* Managed to obtain disk info. */
        {
            size_t i;

            switch (id->id_DiskState) {
                case ID_DISKSTATE_VALIDATING:    /* Consider this an error condition? */
                case ID_DISKSTATE_WRITE_PROTECTED:
                case ID_DISKSTATE_VALIDATED:

                    dos_type = id->id_DiskType;

                    /* See if we know anything about this file-system */
                    for (i = 0; i < NUM_ENTRIES(fs_info); i++) {
                        if (dos_type == fs_info[i].dos_type) {
                            fs_index = i;
                            break;
                        }
                    }

                    break;

                default:

                    SHOWMSG("Invalid disk state.");
                    break;
            }
        } else {
            /* Treat error as no disk present by having default fs_index=0 */
            SHOWMSG("Unable to query DISK_INFO");
        }
    }

    switch (name) {
        case _PC_FILESIZEBITS:
            ret = fs_info[fs_index].file_size_bits;
            break;
        case _PC_LINK_MAX:
            ret = fs_info[fs_index].link_max;
            break;
        case _PC_MAX_CANON:
            ret = 510;    /* I could not find any documentation regarding this. */
            break;
        case _PC_MAX_INPUT: {
                uint32_t Bufsize = 0L;
                struct TagItem TagList[2] =
                    {
                        {DC_FHBufferR, (ULONG) & Bufsize},
                        {TAG_DONE,     0}
                    };
                DosControl(TagList);
                ret = Bufsize;    /* Default is 2048 bytes. */
            }
            break;
        case _PC_NAME_MAX:
            ret = fs_info[fs_index].name_max;
            break;
        case _PC_PATH_MAX:
            ret = fs_info[fs_index].path_max;
            break;
        case _PC_PIPE_BUF:
            ret = 512;    /* One buffer. The PIPE: device usually has 8 of these. */
            break;
        case _PC_XATTR_ENABLED:
            ret = 0;
            break;
        case _PC_XATTR_EXISTS:
            ret = 0;
            break;
        case _PC_CHOWN_RESTRICTED:
            ret = 0;
            break;
        case _PC_NO_TRUNC:
            ret = 0;
            break;
        case _PC_VDISABLE:
            ret = 0;    /* TODO: Implement this in the termios emulation. */
            break;
        case _PC_ASYNC_IO:
            ret = 0;
            break;
        case _PC_PRIO_IO:
            ret = 0;
            break;
        case _PC_SYNC_IO:
            ret = 0;
            break;
        case _PC_SYMLINK_MAX:
            ret = fs_info[fs_index].symlink_max;
            break;
        case _PC_DOSTYPE:    /* Amiga-specific extension. */
            ret = dos_type;
            break;
        default:
            SHOWMSG("Invalid option name");
            __set_errno(EINVAL);
            goto out;
    }

out:

    return (ret);
}
