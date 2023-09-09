/*
 * $Id: ndbm_dbm_fetch.c,v 1.0 2023-07-12 12:04:26 clib2devs Exp $
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
dbm_fetch(DBM *db, datum key) {
    datum retdata;
    int status;
    DBT dbtkey, dbtretdata;

    dbtkey.data = key.dptr;
    dbtkey.size = key.dsize;
    status = (db->get)(db, &dbtkey, &dbtretdata, 0);
    if (status) {
        dbtretdata.data = NULL;
        dbtretdata.size = 0;
    }
    retdata.dptr = dbtretdata.data;
    retdata.dsize = dbtretdata.size;
    return (retdata);
}
