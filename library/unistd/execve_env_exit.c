/*
 * $Id: unistd_execve_env_exit.c,v 1.1 2006-08-02 08:00:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/****************************************************************************/

/* This is a stub which you need override with your own implementation. The
   Function expects a non-NULL pointer to an environment table and will
   do whatever is necessary to clean up after what a previous call to the
   __execve_environ_init() function did. */
void
__execve_environ_exit(char *const envp[]) {
    (void) (envp);
}
