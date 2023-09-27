/*
 * $Id: ndbm_dbm_store.c,v 1.0 2023-07-12 12:04:26 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <ndbm.h>
#include "hash.h"

/*
 * Returns:
 *	 0 on success
 *	<0 failure
 *	 1 if DBM_INSERT and entry exists
 */
int
dbm_store(DBM *db, datum key, datum data, int flags) {
    DBT dbtkey, dbtdata;

    dbtkey.data = key.dptr;
    dbtkey.size = key.dsize;
    dbtdata.data = data.dptr;
    dbtdata.size = data.dsize;
    return ((db->put) (db, &dbtkey, &dbtdata, (flags == DBM_INSERT) ? R_NOOVERWRITE : 0));
}