/*
 * $Id: amiga_deletetask.c,v 1.5 2006-09-25 15:12:47 clib4devs Exp $
*/

#include <proto/exec.h>
#include <exec/libraries.h>
#include <exec/tasks.h>

#if defined(DeleteTask)
#undef DeleteTask
#endif /* DeleteTask */

VOID DeleteTask(struct Task *tc);

VOID
DeleteTask(struct Task *tc) {
    IExec->DeleteTask(tc);
}
