/*
 * $Id: ndbm_dbm_open.c,v 1.0 2023-07-12 12:04:26 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#define __DBINTERFACE_PRIVATE
#include <ndbm.h>
#include <db.h>
#include "hash.h"

/*
 * Returns:
 * 	*DBM on success
 *	 NULL on failure
 */
DBM *
dbm_open(const char *file, int flags, int mode) {
    HASHINFO info;
    char path[MAXPATHLEN];

    info.bsize = 4096;
    info.ffactor = 40;
    info.nelem = 1;
    info.cachesize = 0;
    info.hash = NULL;
    info.lorder = 0;
    (void) strcpy(path, file);
    (void) strcat(path, DBM_SUFFIX);
    return ((DBM *) __hash_open(path, flags, mode, &info, 0));
}