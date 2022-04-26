/*
 * $Id: amiga_acrypt.c,v 1.5 2006-09-25 15:12:47 clib2devs Exp $
 */

#define ACrypt __ACrypt

#include <exec/types.h>
#include <string.h>
#include <clib/alib_protos.h>

#undef ACrypt

#include "debug.h"

#define OSIZE 12

UBYTE *ACrypt(UBYTE *buffer, const UBYTE *password, const UBYTE *user);

UBYTE *
ACrypt(UBYTE *buffer, const UBYTE *password, const UBYTE *user) {
    UBYTE *result = NULL;
    LONG buf[OSIZE];
    LONG i, d, k;

    ENTER();

    assert(buffer != NULL && password != NULL && user != NULL);

    SHOWPOINTER((char *)buffer);
    SHOWSTRING((char *)password);
    SHOWSTRING((char *)user);

    if (buffer == NULL || password == NULL || user == NULL) {
        SHOWMSG("invalid parameters");
        goto out;
    }

    for (i = 0; i < OSIZE; i++) {
        if ((*password) != '\0')
            d = (*password++);
        else
            d = i;

        if ((*user) != '\0')
            d += (*user++);
        else
            d += i;

        buf[i] = 'A' + d;
    }

    for (i = 0; i < OSIZE; i++) {
        for (k = 0; k < OSIZE; k++)
            buf[i] = (buf[i] + buf[OSIZE - k - 1]) % 53;

        buffer[i] = buf[i] + 'A';
    }

    buffer[OSIZE - 1] = '\0';

    SHOWSTRING((char *)buffer);

    result = buffer;

out:

    RETURN(result);
    return (result);
}
