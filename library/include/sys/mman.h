/*
 * $Id: mman.h,v 1.0 2021-01-18 20:17:27 apalmate Exp $
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
 */

#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <features.h>

__BEGIN_DECLS

#include <sys/types.h> // off_t

/* Return value of `mmap' in case of an error.  */
#define MAP_FAILED      ((void *) -1)

#define PROT_READ       0x1             /* page can be read */
#define PROT_WRITE      0x2             /* page can be written */
#define PROT_EXEC       0x4             /* page can be executed */
#define PROT_SEM        0x8             /* page may be used for atomic ops */
#define PROT_NONE       0x0             /* page can not be accessed */

#define MAP_SHARED      0x01            /* Share changes */
#define MAP_PRIVATE     0x02            /* Changes are private */
#define MAP_SHARED_VALIDATE 0x03        /* share + validate extension flags */
#define MAP_TYPE        0x0f            /* Mask for type of mapping */
#define MAP_FIXED       0x10            /* Interpret addr exactly */
#define MAP_ANONYMOUS   0x20            /* don't use a file */

extern void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
extern int munmap(void *map, size_t length);

__END_DECLS

#endif // _SYS_MMAN_H