/*
 * $Id: time_tzvars.c,v 1.0 2024-05-01 12:04:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

/* Global timezone variables.  */

/* Default timezone to GMT */
char *_tzname[2] = {"GMT", "GMT"}; /* Current timezone names.  */
int _daylight = 0;                      /* If daylight-saving time is ever in use.  */
long _timezone = 0;                     /* Seconds west of UTC.  */