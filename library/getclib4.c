#include <proto/exec.h>
#include <dos/dosextens.h>
#include <dos.h>
#include <proto/dos.h>
#include <debug.h>

#include "shared_library/clib4.h"

struct _clib4 *
__getClib4(void) {
    struct _clib4 *r = NULL;
    struct Task *t = FindTask(NULL);

    if (NT_PROCESS == t->tc_Node.ln_Type) {
#if 0 //This is too slow at moment..
        uint32 value;
        /* Get _clib4 address stored into process pr_UID field */
        GetOwnerInfoTags(OI_ProcessInput, 0, OI_OwnerUID, &value, TAG_END);
        r = (struct _clib4 *) value;
#else
        r = (struct _clib4 *) ((struct Process *)t)->pr_UID;
#endif
    }

    if (!r) {
        struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);
        if (res) {
            return res->fallbackClib;
        }
    }
    return r;
}
