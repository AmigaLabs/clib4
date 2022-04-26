/*
 * $Id: stdlib_set_process_window.c,v 1.3 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

APTR
__set_process_window(APTR new_window_pointer) {
    APTR result;

    ENTER();
    SHOWPOINTER(new_window_pointer);

    result = SetProcWindow(new_window_pointer);

    SHOWPOINTER(result);
    LEAVE();

    return (result);
}
