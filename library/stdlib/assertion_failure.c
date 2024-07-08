/*
 * $Id: stdlib_assertion_failure.c,v 1.12 2006-01-08 12:04:25 clib4devs Exp $
*/

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

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
__assertion_failure(const char *file_name, int line_number, const char *expression) {
    static int been_here_before;
    struct _clib4 *__clib4 = __CLIB4;

    /* Don't drop into a recursion. */
    if (been_here_before++ == 0) {
        if (__clib4->__no_standard_io || __clib4->__WBenchMsg != NULL) {
            struct IntuitionIFace *IIntuition = NULL;
            struct Library *IntuitionBase;

            IntuitionBase = OpenLibrary("intuition.library", 37);
            if (IntuitionBase != NULL) {
                IIntuition = (struct IntuitionIFace *) GetInterface(IntuitionBase, "main", 1, 0);
                if (IIntuition == NULL) {
                    CloseLibrary(IntuitionBase);
                    IntuitionBase = NULL;
                }
            }

            if (IntuitionBase != NULL) {
                struct EasyStruct es;

                memset(&es, 0, sizeof(es));

                es.es_StructSize = sizeof(es);
                es.es_Title = (STRPTR) __clib4->__progname;
                es.es_TextFormat = (STRPTR) "Assertion of condition\n\"%s\"\nfailed in file \"%s\", line %ld.";
                es.es_GadgetFormat = (STRPTR) "Sorry";

                EasyRequest(NULL, &es, NULL, expression, file_name, line_number);

                DropInterface((struct Interface *) IIntuition);
                CloseLibrary(IntuitionBase);
            }
        } else {
            if (__clib4->__num_iob > STDERR_FILENO) {
                if (__clib4->__progname != NULL)
                    fprintf(stderr, "[%s] ", __clib4->__progname);

                fprintf(stderr,
                        "%s:%d: failed assertion \"%s\".\n",
                        file_name,
                        line_number,
                        expression);

                abort();
            } else {
                struct DOSIFace *IDOS = NULL;
                struct Library *DOSBase;

                DOSBase = OpenLibrary("dos.library", 37);
                if (DOSBase != NULL) {
                    IDOS = (struct DOSIFace *) GetInterface(DOSBase, "main", 1, 0);
                    if (IDOS == NULL) {
                        CloseLibrary(DOSBase);
                        DOSBase = NULL;
                    }
                }

                if (DOSBase != NULL) {
                    BPTR output;

                    /* Dump all currently unwritten data, especially to the console. */
                    __flush_all_files(__clib4, -1);

                    /* Try to print the error message on the default error output stream. */
                    output = ErrorOutput();

                    if (output == BZERO)
                        output = Output();

                    if (output != BZERO) {
                        if (__clib4->__progname != NULL)
                            FPrintf(output, "[%s] ", __clib4->__progname);

                        FPrintf(output,
                                "%s:%ld: failed assertion \"%s\".\n",
                                file_name,
                                line_number,
                                expression);
                    }

                    DropInterface((struct Interface *) IDOS);
                    CloseLibrary(DOSBase);
                }

                _exit(EXIT_FAILURE);
            }
        }
    }

    been_here_before--;
}
