/*
 * $Id: ndbm_hash_log2.c,v 1.0 2023-07-12 12:04:26 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <db.h>
#include "hash.h"
#include "page.h"
#include "extern.h"

uint32_t
__log2(uint32_t num) {
    uint32_t i, limit;

    limit = 1;
    for (i = 0; limit < num; limit = limit << 1, i++);
    return (i);
}