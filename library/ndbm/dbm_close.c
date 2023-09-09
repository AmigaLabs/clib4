/*
 * $Id: ndbm_dbm_close.c,v 1.0 2023-07-12 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <ndbm.h>
#include "hash.h"

void
dbm_close(DBM *db) {
    (void) (db->close)(db);
}