/*
 * $Id: amiga_invertstring.c,v 1.5 2021-01-31 12:04:22 clib2devs Exp $
*/

#include <libraries/commodities.h>
#include <devices/keymap.h>
#include <exec/memory.h>

#include <string.h>

#include <proto/exec.h>
#include <proto/commodities.h>
#include <clib/alib_protos.h>

#include "debug.h"

STATIC int
do_escape(int cc) {
    int result;

    switch (cc) {
        case 'n':
        case 'r':

            result = '\r';
            break;

        case 't':

            result = '\t';
            break;

        case '0':

            result = '\0';
            break;

        case '\\':
        case '\"':
        case '\'':
        case '<':

            result = cc;
            break;

        default:

            result = -1;
            break;
    }

    return (result);
}


STATIC ULONG
do_angle(STRPTR *strp, struct InputEvent *ie)
{
    ULONG result;
    char *cp;
    IX ix;

    /* find closing angle '>', put a null there   */
    for (cp = (*strp); (*cp); cp++) {
        if ((*cp) == '>')
        {
            (*cp) = '\0';
            break;
        }
    }

    result = ParseIX((*strp), &ix);

    if (cp != NULL)
    {
        (*cp) = '>';  /* fix it */
        (*strp) =
        cp; /* point to char following '>' */
    }
    else
    {
        (*strp) = cp - 1; /* ++will point to terminating null */
    }

    if (result == 0)
    {
        /* use IX to describe a suitable InputEvent */
        ie->ie_Class = ix.ix_Class;
        ie->ie_Code = ix.ix_Code;
        ie->ie_Qualifier = ix.ix_Qualifier;
    }

    return (result);
}

extern VOID FreeIEvents(struct InputEvent *ie);

struct InputEvent *
InvertString(CONST_STRPTR str, CONST struct KeyMap *km) {
    /* bugs:
       can't escape '>'
       puts '\0' on closing angles */

    /* allocate input event for each character in string   */

    struct InputEvent *result = NULL;
    struct InputEvent *chain = NULL;
    struct InputEvent *ie;
    int cc;

    if (CxBase == NULL || str == NULL || (*str) == '\0')
        goto out;

    do /* have checked that str is not null    */
    {
        /* allocate the next ie and link it in */
        ie = AllocVecTags(sizeof(*ie), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_DONE);
        if (ie == NULL)
            goto out;

        ie->ie_NextEvent = chain;
        chain = ie;

        /* now fill in the input event   */
        cc = (*str);
        switch (cc) {
            case '<':

                str++;

                if (do_angle((STRPTR * ) & str, ie) != 0)
                    goto out;

                break;

            case '\\':

                str++;

                cc = do_escape(*str); /* get escaped character */
                if (cc < 0)
                    goto out;

                /* fall through  */

            default:

                InvertKeyMap((ULONG)
                cc, ie, (struct KeyMap *) km);
                break;
        }

        str++;
    } while ((*str) != '\0');

    result = chain;

out:

    if (result == NULL && chain != NULL)
        FreeIEvents(chain);

    return (result);
}
