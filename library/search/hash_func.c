/*
 * $Id: search_hash_func.c,v 1.0 2021-02-21 19:38:14 clib4devs Exp $
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
