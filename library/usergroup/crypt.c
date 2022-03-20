/*
 * $Id: usergroup_crypt.c,v 1.6 2006-09-25 15:12:47 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H

#include "usergroup_headers.h"

#endif /* _USERGROUP_HEADERS_H */

#ifdef crypt
#undef crypt
#endif /* crypt */

/****************************************************************************/

char *
crypt(const char *key, const char *salt) {
    char *result = NULL;

    ENTER();

    SHOWSTRING(key);
    SHOWSTRING(salt);

    assert(key != NULL && salt != NULL);
    assert(__UserGroupBase != NULL);

    if (key == NULL || salt == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    result = (char *) __crypt((UBYTE *) key, (UBYTE *) salt);

    SHOWSTRING(result);

out:

    if (__check_abort_enabled)
        __check_abort();

    RETURN(result);
    return (result);
}
