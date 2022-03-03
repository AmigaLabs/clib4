/*
 * $Id: simple_sprintf.c,v 1.2 2005-05-18 07:22:52 obarthel Exp $
 *
 * :ts=4
 */

#include <exec/execbase.h>

#define __NOLIBBASE__
#define __USE_INLINE__
#define __NOGLOBALIFACE__

#include <proto/exec.h>
#include <proto/dos.h>

#include <string.h>
#include <stdio.h>
#include <dos.h>

struct Library *SysBase;
struct Library *DOSBase;

struct ExecIFace *IExec;
struct DOSIFace *IDOS;

BOOL __check_abort_enabled = FALSE;

int _start(void);

int
_start(void) {
    char string[80];

    SysBase = *(struct Library **) 4;
    IExec = (struct ExecIFace *) ((struct ExecBase *) SysBase)->MainInterface;

    DOSBase = OpenLibrary("dos.library", 37);
    if (DOSBase == NULL)
        goto out;

    IDOS = (struct DOSIFace *) GetInterface(DOSBase, "main", 1, 0);
    if (IDOS == NULL)
        goto out;

    sprintf(string, "a %s c\n", "b");

    Write(Output(), string, (LONG) strlen(string));

    out:

    if (IDOS != NULL)
        DropInterface((struct Interface *) IDOS);

    if (DOSBase != NULL)
        CloseLibrary(DOSBase);

    return (0);
}

/****************************************************************************/

void
__check_abort(void) {
}
