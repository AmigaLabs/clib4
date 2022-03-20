/*
 * $Id: search_lsearch.c,v 1.0 2021-02-21 19:38:14 clib2devs Exp $
*/

#include <search.h>
#include <string.h>

void *
lfind(const void *key, const void *base, size_t *nelp, size_t width, int (* compar)(const void *, const void *))
{
   uint8_t *cur, *end;

   end = (uint8_t *)base + *nelp * width;

   for (cur = (uint8_t *)base; cur < end; cur += width)
   {
      if (!compar(cur, key))
      {
         return cur;
      }
   }

   return NULL;
}
