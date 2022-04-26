/*
 * $Id: termios_cfgetispeed.c,v 1.2 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef    _TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif /* _TERMIOS_HEADERS_H */

speed_t
cfgetispeed(const struct termios *tios) {
    speed_t result = ERROR;

    ENTER();

    SHOWPOINTER(tios);

    if (tios == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = tios->c_ispeed;

out:

    RETURN(result);
    return (result);
}
