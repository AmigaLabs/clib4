/*
 * $Id: string_strcasestr.c,v 1.0 2021-02-23 10:01:27 clib4devs Exp $
*/

#include <string.h>
#include <ctype.h>

char *
strcasestr(const char *psz_big, const char *psz_little)
{
    char *p_pos = (char *)psz_big;

    if (!*psz_little)
        return p_pos;

    while (*p_pos)
    {
        if (toupper((unsigned char)*p_pos) == toupper((unsigned char)*psz_little))
        {
            char *cur1 = p_pos + 1;
            char *cur2 = (char *)psz_little + 1;
            while (*cur1 && *cur2 && toupper((unsigned char)*cur1) == toupper((unsigned char)*cur2))
            {
                cur1++;
                cur2++;
            }
            if (!*cur2)
                return p_pos;
        }
        p_pos++;
    }
    return NULL;
}
