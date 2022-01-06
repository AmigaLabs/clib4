/*
 * crtbegin.c
 *
 * :ts=4
 *
 * Handles global constructors and destructors for the OS4 GCC build.
 *
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

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/elf.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

/*
 * Dummy constructor and destructor array. The linker script will put these at the
 * very beginning of section ".ctors" and ".dtors". crtend.o contains a similar entry
 * with a NULL pointer entry and is put at the end of the sections. This way, the init
 * code can find the global constructor/destructor pointers.
 *
 * WARNING:
 * This hack does not work correctly with GCC 5 and higher. The optimizer
 * will see a one element array and act appropriately. The current workaround
 * is to use -fno-aggressive-loop-optimizations when compiling this file.
 */

static void (*__CTOR_LIST__[1])(void) __attribute__((used, section(".ctors"), aligned(sizeof(void (*)(void)))));
static void (*__DTOR_LIST__[1])(void) __attribute__((used, section(".dtors"), aligned(sizeof(void (*)(void)))));

void _init(void);
void _fini(void);
void shared_obj_init(void);
void shared_obj_exit(void);

#include <libraries/elf.h>
#include <proto/elf.h>

/* These are used to initialize the shared objects linked to this binary,
   and for the dlopen(), dlclose() and dlsym() functions. */
struct Library  *__ElfBase;
struct ElfIFace *__IElf;

void
_init(void) {
    int num_ctors, i;
    int j;
    Printf("_init\n");

    for (i = 1, num_ctors = 0; __CTOR_LIST__[i] != NULL; i++)
        num_ctors++;
    Printf("num_ctors=%ld\n", num_ctors);
    if (num_ctors > 0) {
        for (j = 0; j < num_ctors; j++) {
            Printf("j=%ld\n", j);
            __CTOR_LIST__[num_ctors - j]();
        }
    }
}

/****************************************************************************/

void
_fini(void) {
    int num_dtors, i;
    static int j;

    for (i = 1, num_dtors = 0; __DTOR_LIST__[i] != NULL; i++)
        num_dtors++;

    if (num_dtors > 0) {
        while (j++ < num_dtors)
            __DTOR_LIST__[j]();
    }

    /* The shared objects need to be cleaned up after all local
       destructors have been invoked. */
    shared_obj_exit();
}

static void SHLibsInit(BOOL init) {
    struct ElfIFace *IElf = __IElf;

    Elf32_Handle hSelf = (Elf32_Handle) NULL;
    BPTR segment_list = GetProcSegList(NULL, GPSLF_CLI | GPSLF_SEG);
    if (segment_list != ZERO) {
        int ret = GetSegListInfoTags(segment_list, GSLI_ElfHandle, &hSelf, TAG_DONE);
        if (ret == 1) {
            if (hSelf != NULL) {
                /* Trigger the constructors, etc. in the shared objects linked to this binary. */
                InitSHLibs(hSelf, init);
            }
        }
    }
}

void shared_obj_exit(void)
{
    /* If we got what we wanted, trigger the destructors, etc. in the shared objects linked to this binary. */
    SHLibsInit(FALSE);
}

void shared_obj_init()
{
    SHLibsInit(TRUE);
}