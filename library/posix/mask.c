/*
 * $Id: signal_mask.c,v 1.1 2006-04-05 08:39:45 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

/****************************************************************************/

ULONG __break_signal_mask = SIGBREAKF_CTRL_C;
