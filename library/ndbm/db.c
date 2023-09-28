/*
 * $Id: ndbm_db.c,v 1.0 2023-07-12 09:02:51 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#undef __BSD_VISIBLE
#define __BSD_VISIBLE 1
#define __DBINTERFACE_PRIVATE

#include <db.h>

static int __dberr(void);

DB *
dbopen(const char *fname, int flags, int mode, DBTYPE type, const void *openinfo) {
#define    DB_FLAGS    (DB_LOCK | DB_SHMEM | DB_TXN)
#define    USE_OPEN_FLAGS (O_CREAT | O_EXCL | O_NONBLOCK | O_RDONLY | O_RDWR | O_SYNC | O_TRUNC)
    if ((flags & ~(USE_OPEN_FLAGS | DB_FLAGS)) == 0)
        switch (type) {
            case DB_BTREE:
                //return (__bt_open(fname, flags & USE_OPEN_FLAGS, mode, openinfo, flags & DB_FLAGS));
                __set_errno(ENOSYS);
                return NULL;
            case DB_HASH:
                return (__hash_open(fname, flags & USE_OPEN_FLAGS, mode, openinfo, flags & DB_FLAGS));
            case DB_RECNO:
                //return (__rec_open(fname, flags & USE_OPEN_FLAGS, mode, openinfo, flags & DB_FLAGS));
                __set_errno(ENOSYS);
                return NULL;
        }
    __set_errno(EINVAL);
    return (NULL);
}

static int
__dberr(void) {
    return (RET_ERROR);
}

/*
 * __DBPANIC -- Stop.
 *
 * Parameters:
 *	dbp:	pointer to the DB structure.
 */
void
__dbpanic(DB *dbp) {
    /* The only thing that can succeed is a close. */
    dbp->del = (int (*)(const struct __db *, const DBT *, u_int)) __dberr;
    dbp->fd = (int (*)(const struct __db *)) __dberr;
    dbp->get = (int (*)(const struct __db *, const DBT *, DBT *, u_int)) __dberr;
    dbp->put = (int (*)(const struct __db *, DBT *, const DBT *, u_int)) __dberr;
    dbp->seq = (int (*)(const struct __db *, DBT *, DBT *, u_int)) __dberr;
    dbp->sync = (int (*)(const struct __db *, u_int)) __dberr;
}