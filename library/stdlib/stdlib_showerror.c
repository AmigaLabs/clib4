/*
 * $Id: stdlib_showerror.c,v 1.14 2006-09-25 15:12:47 obarthel Exp $
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

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif /* EXEC_LIBRARIES_H */

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif /* INTUITION_INTUITION_H */

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif /* DOS_DOS_H */

#ifndef WORKBENCH_STARTUP_H
#include <workbench/startup.h>
#endif /* WORKBENCH_STARTUP_H */

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif /* PROTO_EXEC_H */

#define __NOLIBBASE__
#define __NOGLOBALIFACE__

#ifndef PROTO_INTUITION_H
#include <proto/intuition.h>
#endif /* PROTO_INTUITION_H */

#ifndef PROTO_DOS_H
#include <proto/dos.h>
#endif /* PROTO_DOS_H */

#include <string.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void __show_error(const char *message) {
    struct IntuitionIFace *IIntuition = NULL;
    struct DOSIFace *IDOS = NULL;

    struct Library *IntuitionBase = NULL;
    struct Library *DOSBase = NULL;

    PROFILE_OFF();

    /* Don't show anything if this is the thread-safe library and
       we were invoked indirectly by shared library startup code. */
    if (__lib_startup)
        goto out;

    DOSBase = OpenLibrary("dos.library", 0);
    IntuitionBase = OpenLibrary("intuition.library", 0);

    if (DOSBase == NULL || IntuitionBase == NULL)
        goto out;

    IDOS = (struct DOSIFace *) GetInterface(DOSBase, "main", 1, 0);
    if (IDOS == NULL)
        goto out;

    IIntuition = (struct IntuitionIFace *) GetInterface(IntuitionBase, "main", 1, 0);
    if (IIntuition == NULL)
        goto out;

    /* If we can't hope to print the error message, show a requester instead. */
    if (__no_standard_io || __WBenchMsg != NULL) {
        UBYTE program_name[256] = {0};
        struct EasyStruct es;
        STRPTR title_string;

        if (__WBenchMsg != NULL) {
            title_string = (STRPTR) FilePart(__WBenchMsg->sm_ArgList[0].wa_Name);
        } else {
            if (GetCliProgramName((STRPTR) program_name, sizeof(program_name)))
                title_string = (STRPTR) FilePart((STRPTR) program_name);
            else
                title_string = (STRPTR) "Error";
        }

        memset(&es, 0, sizeof(es));

        es.es_StructSize = sizeof(es);
        es.es_Title = title_string;
        es.es_TextFormat = (STRPTR) message;
        es.es_GadgetFormat = (STRPTR) "Sorry";

        EasyRequestArgs(NULL, &es, NULL, NULL);
    } else {
        BPTR output;

        /* Try to print the error message on the default error output stream. */
        output = ErrorOutput();

        if (output == ZERO)
            output = Output();

        if (output != ZERO) {
            Write(output, (STRPTR) message, (LONG) strlen(message));
            Write(output, "\n", 1);
        }
    }

out:

    if (IIntuition != NULL)
        DropInterface((struct Interface *) IIntuition);

    if (IDOS != NULL)
        DropInterface((struct Interface *) IDOS);

    if (IntuitionBase != NULL)
        CloseLibrary(IntuitionBase);

    if (DOSBase != NULL)
        CloseLibrary(DOSBase);

    PROFILE_ON();
}
