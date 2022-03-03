/*
 * $Id: amiga_createtask.c,v 1.6 2006-09-25 15:12:47 obarthel Exp $
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

#include <proto/exec.h>

#include <exec/libraries.h>
#include <exec/memory.h>
#include <exec/tasks.h>

#include <string.h>

#include "macros.h"
#include "debug.h"

#if defined(CreateTask)
#undef CreateTask
#endif /* CreateTask */

struct Task *CreateTask(CONST_STRPTR name, LONG pri, CONST APTR init_pc, ULONG stack_size);

struct Task *
CreateTask(CONST_STRPTR name, LONG pri, CONST APTR init_pc, ULONG stack_size) {
    struct Task *result = NULL;

    ENTER();

    SHOWSTRING(name);
    SHOWVALUE(pri);
    SHOWPOINTER(init_pc);
    SHOWVALUE(stack_size);

    assert(name != NULL && (-128 <= pri && pri <= 127) && init_pc != NULL && stack_size > 0);

    if (name == NULL || pri < -128 || pri > 127 || init_pc == NULL || stack_size == 0) {
        SHOWMSG("invalid parameters");
        goto out;
    }

    result = IExec->CreateTask(name, pri, init_pc, stack_size, NULL);

    out:

    RETURN(result);
    return (result);
}
