#ifndef _SYS_STATVFS_H
#define _SYS_STATVFS_H

#include <features.h>
#include <sys/types.h>

#define ST_RDONLY         0x0001 /* Read-only file system. */
#define ST_NOSUID         0x0002 /* Does not support the semantics of the ST_ISUID and ST_ISGID file mode bits. */
#define ST_CASE_SENSITIVE 0x0004 /* The file system is case sensitive. */

#define ST_WAIT           1
#define ST_NOWAIT         2

__BEGIN_DECLS

struct statvfs {
    unsigned long f_bsize;
    unsigned long f_frsize;
    fsblkcnt_t f_blocks;
    fsblkcnt_t f_bfree;
    fsblkcnt_t f_bavail;

    fsfilcnt_t f_files;
    fsfilcnt_t f_ffree;
    fsfilcnt_t f_favail;

    unsigned long f_fsid;
    unsigned long f_flag;
    unsigned long f_namemax;

    char f_mntonname[256];  // Mount point
    char f_fstypename[32];  // File system type
};

extern int statvfs(const char *, struct statvfs *);
extern int fstatvfs(int, struct statvfs *);
extern int getvfsstat(struct statvfs *buf, size_t bufsize, int flags);

__END_DECLS


#endif /* _SYS_STATVFS_H */
