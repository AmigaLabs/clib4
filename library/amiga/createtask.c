/*
 * $Id: amiga_createtask.c,v 1.6 2006-09-25 15:12:47 clib2devs Exp $
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
