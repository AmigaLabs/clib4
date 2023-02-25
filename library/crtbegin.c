/*
* $Id: crtbegin.c,v 1.3 2022-03-09 21:07:25 clib2devs Exp $
  */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/elf.h>
#include <libraries/elf.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

int _start(char *args, int arglen, struct ExecBase *sysBase) {
    SysBase = (struct Library *) sysBase;
    IExec = (struct ExecIFace *) ((struct ExecBase *) SysBase)->MainInterface;

    /* The shared objects need to be set up before any local constructors are invoked. */
    shared_obj_init();

    int r = _main(args, arglen, sysBase);

    /* The shared objects need to be cleaned up after all local destructors have been invoked. */
    shared_obj_exit();

    return r;
}