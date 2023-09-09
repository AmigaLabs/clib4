/*
 * $Id: ndbm_dbm_delete.c,v 1.0 2023-07-12 12:04:26 clib2devs Exp $
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
 */
int
dbm_delete(DBM *db, datum key) {
    int status;
    DBT dbtkey;

    dbtkey.data = key.dptr;
    dbtkey.size = key.dsize;
    status = (db->del)(db, &dbtkey, 0);
    if (status)
        return (-1);
    else
        return (0);
}