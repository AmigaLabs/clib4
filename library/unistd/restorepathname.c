/*
 * $Id: unistd_restorepathname.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/****************************************************************************/

void
__restore_path_name(char const **name_ptr, struct name_translation_info *nti) {
    assert(name_ptr != NULL && (*name_ptr) != NULL && nti != NULL);

    /* Reset the name pointer to its previous position. */
    (*name_ptr) = nti->original_name;
}
