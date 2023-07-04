#include <proto/exec.h>
#include <dos/dosextens.h>
#include <dos.h>
#include <proto/dos.h>
#include <debug.h>

#include "shared_library/clib2.h"
#include "map.h"

struct _clib2 *
__getClib2(void) {
    struct _clib2 *r = NULL;

    struct Task *t = FindTask(NULL);
    if (NT_PROCESS == t->tc_Node.ln_Type) {
        r = (struct _clib2 *) ((struct Process *) t)->pr_UID;
    }

    return r;
}
