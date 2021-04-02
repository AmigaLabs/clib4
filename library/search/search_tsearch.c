/*
 * $Id: search_tsearch.c,v 1.0 2021-02-21 19:38:14 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 */

#define _SEARCH_PRIVATE
#include <search.h>
#include <string.h>
#include <stdlib.h>

/* find or insert datum into search tree */
void *
tsearch(const void *vkey, void **vrootp, int (*compar)(const void *, const void *))
{
    node_t *q;
    node_t **rootp = (node_t **)vrootp;

    if (rootp == NULL)
        return NULL;

    while (*rootp != NULL)
    { /* Knuth's T1: */
        int r;

        if ((r = (*compar)(vkey, (*rootp)->key)) == 0) /* T2: */
            return *rootp;                             /* we found it! */

        rootp = (r < 0) ? &(*rootp)->llink : /* T3: follow left branch */
                    &(*rootp)->rlink;        /* T4: follow right branch */
    }

    q = malloc(sizeof(node_t)); /* T5: key not found */
    if (q != 0)
    {               /* make new node */
        *rootp = q; /* link new node to old */
        /* LINTED const castaway ok */
        q->key = (void *)vkey; /* initialize new node */
        q->llink = q->rlink = NULL;
    }
    return q;
}
