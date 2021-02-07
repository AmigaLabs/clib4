/*
 * $Id: dirent.h,v 1.7 2006-01-08 12:06:14 obarthel Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
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
    ino_t          d_ino;               /* Inode number */
    off_t          d_off;               /* Not an offset; see below */
    unsigned short d_reclen;            /* Length of this record */
    unsigned char  d_type;              /* Type of file; not supported by all filesystem types */
    char           d_name[NAME_MAX+1];  /* Null-terminated filename */
};

extern DIR * opendir(const char * path_name);
extern struct dirent * readdir(DIR * directory_pointer);
extern void rewinddir(DIR * directory_pointer);
extern int closedir(DIR * directory_pointer);
extern int alphasort(const struct dirent **a, const struct dirent **b);
/****************************************************************************/

__END_DECLS

#endif /* _DIRENT_H */
