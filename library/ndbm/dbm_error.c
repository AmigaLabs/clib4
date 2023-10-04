/*
 * $Id: ndbm_dbm_error.c,v 1.0 2023-07-12 12:04:26 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <ndbm.h>
#include "hash.h"

int
dbm_error(DBM *db) {
    HTAB *hp;

    hp = (HTAB *) db->internal;
    return (hp->error);
}