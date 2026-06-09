/*
 * $Id: stdio_findfp.c,v 1.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * Stream allocation using a newlib-inspired glue-list model.
 *
 * __sfp()   — find a free FILE slot (walk per-process glue list, grow if needed)
 * __sinit() — lazy one-time stdio initialization
 *
 * The root glue node and the stdin/stdout/stderr iob structs are now
 * per-process fields in struct _clib4 (__sglue and __sf[3]), allocated by
 * stdio_file_init and freed by __close_all_files.  The old shared globals
 * __sf[3] / __sf_ptrs[3] / __sglue have been removed.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

/****************************************************************************/

#define NDYNAMIC 8  /* Number of FILE slots per dynamically-allocated glue block */

/*
 * Allocate a new glue block with `n` iob slots.
 * Returns the glue block or NULL on failure.
 */
static struct _glue *
sfmoreglue(struct _clib4 *__clib4, int n) {
    struct _glue *g;
    struct iob *p;
    struct iob **ptrs;
    int i;

    ENTER();

    g = (struct _glue *) malloc(sizeof(struct _glue));
    if (g == NULL)
        goto out;

    ptrs = (struct iob **) calloc(n, sizeof(struct iob *));
    if (ptrs == NULL) {
        free(g);
        g = NULL;
        goto out;
    }

    p = (struct iob *) calloc(n, sizeof(struct iob));
    if (p == NULL) {
        free(ptrs);
        free(g);
        g = NULL;
        goto out;
    }

    for (i = 0; i < n; i++) {
        ptrs[i] = &p[i];
    }

    g->next = NULL;
    g->niobs = n;
    g->iobs = ptrs;

out:
    RETURN(g);
    return g;
}

/****************************************************************************/

/*
 * __sfp — Find a free FILE slot.
 *
 * Walks the glue list looking for a slot with IOBF_IN_USE clear.
 * If none found, allocates a new glue block with NDYNAMIC slots.
 * Returns a pointer to the iob struct, or NULL on failure (sets errno).
 *
 * The caller must hold the stdio_lock.
 */
struct iob *
__sfp(struct _clib4 *__clib4) {
    struct iob *fp;
    struct _glue *g;
    int n;

    ENTER();

    if (__clib4->__sglue == NULL) {
        __set_errno(ENOMEM);
        fp = NULL;
        goto out;
    }

    for (g = __clib4->__sglue; g != NULL; g = g->next) {
        for (n = 0; n < g->niobs; n++) {
            fp = g->iobs[n];
            if (fp != NULL && FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE)) {
                /* Found a free slot — clear it for reuse */
                memset(fp, 0, sizeof(*fp));
                SET_FLAG(fp->iob_Flags, IOBF_IN_USE);
                goto found;
            }
        }
    }

    /* No free slot found — allocate a new glue block */
    g = sfmoreglue(__clib4, NDYNAMIC);
    if (g == NULL) {
        __set_errno(ENOMEM);
        fp = NULL;
        goto out;
    }

    /* Link the new block at the end of the chain */
    {
        struct _glue *last;
        for (last = __clib4->__sglue; last->next != NULL; last = last->next)
            ;
        last->next = g;
    }

    /* Use the first slot from the new block */
    fp = g->iobs[0];
    memset(fp, 0, sizeof(*fp));
    SET_FLAG(fp->iob_Flags, IOBF_IN_USE);

found:

    /* Initialize the new slot defaults */
    fp->_cookie = fp;  /* Default: cookie points to the iob itself */
    fp->_ub._base = NULL;
    fp->_ub._size = 0;
    fp->_lb._base = NULL;
    fp->_lb._size = 0;
    fp->_offset = 0;
    fp->_blksize = BUFSIZ;

out:
    RETURN(fp);
    return fp;
}

/****************************************************************************/

/*
 * __sinit — One-time stdio initialization.
 *
 * Called by CHECK_INIT() on first stdio use. Sets the
 * __stdio_initialized flag in _clib4 so it runs only once.
 *
 * This does NOT set up stdin/stdout/stderr — that is done by
 * file_init.c's FILE_CONSTRUCTOR which runs before main().
 * __sinit() is a safety net for any stdio call that happens
 * before or outside the constructor path.
 */
void
__sinit(struct _clib4 *__clib4) {
    ENTER();

    if (__clib4->__stdio_initialized) {
        LEAVE();
        return;
    }

    /* Mark as initialized. The actual stream setup for stdin/stdout/stderr
     * is handled by the FILE_CONSTRUCTOR in file_init.c. */
    __clib4->__stdio_initialized = 1;

    LEAVE();
}
