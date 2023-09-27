/*
 * $Id: ndbm_dbm_firstkey.c,v 1.0 2023-07-12 12:04:26 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <ndbm.h>
#include "hash.h"

/*
 * Returns:
 *	DATUM on success
 *	NULL on failure
 */
datum
dbm_firstkey(DBM *db) {
    int status;
    datum retkey;
    DBT dbtretkey, dbtretdata;

    status = (db->seq)(db, &dbtretkey, &dbtretdata, R_FIRST);
    if (status)
        dbtretkey.data = NULL;
    retkey.dptr = dbtretkey.data;
    retkey.dsize = dbtretkey.size;
    return (retkey);
}