/*
 * $Id: search_hcreate.c,v 1.0 2021-02-21 19:38:14 clib4devs Exp $
*/

#include <search.h>
#include <string.h>
#include <sys/queue.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

static struct hsearch_data htab; // TODO - Move to dos.h!

/* Default hash function, from search_hash_func.c */
extern uint32_t (*__default_hash)(const void *, size_t);

/*
 * DO NOT MAKE THIS STRUCTURE LARGER THAN 32 BYTES (4 ptrs on 64-bit
 * ptr machine) without adjusting MAX_BUCKETS_LG2 below.
 */
struct internal_entry {
    SLIST_ENTRY(internal_entry)
    link;
    ENTRY ent;
};
SLIST_HEAD(internal_head, internal_entry
);

#define MIN_BUCKETS_LG2 4
#define MIN_BUCKETS (1 << MIN_BUCKETS_LG2)

/*
 * max * sizeof internal_entry must fit into size_t.
 * assumes internal_entry is <= 32 (2^5) bytes.
 */
#define MAX_BUCKETS_LG2 (sizeof(size_t) * 8 - 1 - 5)
#define MAX_BUCKETS ((size_t)1 << MAX_BUCKETS_LG2)

int
hcreate_r(size_t nel, struct hsearch_data *_htab) {
    size_t idx;
    unsigned int p2;

    /* Make sure this this isn't called when a table already exists. */
    if (_htab->htable != NULL) {
        __set_errno(EINVAL);
        return 0;
    }

    /* If nel is too small, make it min sized. */
    if (nel < MIN_BUCKETS)
        nel = MIN_BUCKETS;

    /* If it's too large, cap it. */
    if (nel > MAX_BUCKETS)
        nel = MAX_BUCKETS;

    /* If it's is not a power of two in size, round up. */
    if ((nel & (nel - 1)) != 0) {
        for (p2 = 0; nel != 0; p2++)
            nel >>= 1;
        nel = 1 << p2;
    }

    /* Allocate the table. */
    _htab->htablesize = nel;
    _htab->htable = malloc(_htab->htablesize * sizeof _htab->htable[0]);
    if (_htab->htable == NULL) {
        __set_errno(ENOMEM);
        return 0;
    }

    /* Initialize it. */
    for (idx = 0; idx < _htab->htablesize; idx++)
        SLIST_INIT(&(_htab->htable[idx]));

    return 1;
}

int
hsearch_r(ENTRY item, ACTION action, ENTRY **retval, struct hsearch_data *_htab) {
    struct internal_head *head;
    struct internal_entry *ie;
    uint32_t hashval;
    size_t len;

    len = strlen(item.key);
    hashval = (*__default_hash)(item.key, len);

    head = &(_htab->htable[hashval & (_htab->htablesize - 1)]);
    ie = SLIST_FIRST(head);
    while (ie != NULL) {
        if (strcmp(ie->ent.key, item.key) == 0)
            break;
        ie = SLIST_NEXT(ie, link);
    }

    if (ie != NULL) {
        *retval = &ie->ent;
        return 1;
    } else if (action == FIND) {
        *retval = NULL;
        return 0;
    }

    ie = malloc(sizeof *ie);
    if (ie == NULL) {
        *retval = NULL;
        return 0;
    }
    ie->ent.key = item.key;
    ie->ent.data = item.data;

    SLIST_INSERT_HEAD(head, ie, link);
    *retval = &ie->ent;
    return 1;
}

void
hdestroy_r(struct hsearch_data *_htab) {
    if (_htab->htable == NULL)
        return;

    free(_htab->htable);
    _htab->htable = NULL;
}

/************* NON REENTRANT VERSIONS ************/

ENTRY *
hsearch(ENTRY item, ACTION action) {
    ENTRY *retval;

    hsearch_r(item, action, &retval, &htab);

    return retval;
}

void
hdestroy() {
    hdestroy_r(&htab);
}

int
hcreate(size_t nel) {
    return hcreate_r(nel, &htab);
}
