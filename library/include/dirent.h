/*
 * $Id: dirent.h,v 1.7 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef _DIRENT_H
#define _DIRENT_H

#include <features.h>

#include <sys/types.h>
#include <sys/syslimits.h>
#include <stdio.h>

__BEGIN_DECLS

typedef long DIR;

/* Note: each one of these is exactly one more than the high nibble of
   the corresponding S_IF* constant defined on sys/stat.h.  If you
   break that, DTTOIF will stop working!  */
#define DT_REG      0x1
#define DT_BLK      0x2
#define DT_CHR      0x3
#define DT_DIR      0x4
#define DT_FIFO     0x5
#define DT_LABEL    0x6
#define DT_LNK      0x7
#define DT_SOCK     0x8
#define DT_UNKNOWN  0xf

#define DTTOIF(dt)  (((dt) == DT_UNKNOWN ? 0 : (dt) - 1) << 12)

struct dirent {
    ino_t          d_ino;
    unsigned short d_reclen;
    unsigned char  d_type;
    unsigned char  d_namlen;
    char           d_name[NAME_MAX+1];  /* Null-terminated filename */
};

extern DIR * opendir(const char * path_name);
extern struct dirent * readdir(DIR * directory_pointer);
extern void rewinddir(DIR * directory_pointer);
extern int closedir(DIR * directory_pointer);
extern int alphasort(const struct dirent **a, const struct dirent **b);
extern int dirfd(DIR *dirp);

/****************************************************************************/

__END_DECLS

#endif /* _DIRENT_H */
