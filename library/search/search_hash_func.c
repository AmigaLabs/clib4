/*
 * $Id: search_hash_func.c,v 1.0 2021-02-21 19:38:14 apalmate Exp $
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

#include <sys/types.h>
#include <stdint.h>
/*
#include "db_local.h"
#include "hash.h"
#include "page.h"
#include "extern.h"
*/

static uint32_t hash4(const void *, size_t);

/* Global default hash function */
uint32_t (*__default_hash)(const void *, size_t) = hash4;

/* Hash function from Chris Torek. */
static uint32_t
hash4(const void *keyarg, size_t len)
{
    const u_char *key;
    size_t loop;
    uint32_t h;

#define HASH4a h = (h << 5) - h + *key++;
#define HASH4b h = (h << 5) + h + *key++;
#define HASH4 HASH4b

    h = 0;
    key = keyarg;
    if (len > 0)
    {
        loop = (len + 8 - 1) >> 3;

        switch (len & (8 - 1))
        {
        case 0:
            do
            {
                HASH4;
                /* FALLTHROUGH */
            case 7:
                HASH4;
                /* FALLTHROUGH */
            case 6:
                HASH4;
                /* FALLTHROUGH */
            case 5:
                HASH4;
                /* FALLTHROUGH */
            case 4:
                HASH4;
                /* FALLTHROUGH */
            case 3:
                HASH4;
                /* FALLTHROUGH */
            case 2:
                HASH4;
                /* FALLTHROUGH */
            case 1:
                HASH4;
            } while (--loop);
        }
    }
    return (h);
}
