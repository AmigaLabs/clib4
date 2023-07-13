/*
 * $Id: ndbm_dbm_dirfno.c,v 1.0 2023-07-12 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#undef __BSD_VISIBLE
#define __BSD_VISIBLE 1
#include <ndbm.h>
#include "hash.h"

int
dbm_dirfno(DBM *db) {
    return(((HTAB *) db->internal)->fp);
}