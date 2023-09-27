/*
 * $Id: ndbm_dbm_clearerr.c,v 1.0 2023-07-12 12:04:26 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <ndbm.h>
#include "hash.h"

int
dbm_clearerr(DBM *db) {
    HTAB *hp;

    hp = (HTAB *) db->internal;
    hp->error = 0;
    return (0);
}