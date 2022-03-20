/*
 * $Id: unistd_execve_env_init.c,v 1.1 2006-08-02 08:00:28 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/****************************************************************************/

/* This is a stub which you need override with your own implementation. The
   Function expects a non-NULL pointer to an environment table and will
   do whatever is necessary to prepare a subsequent call to RunCommand()
   to use the environment data. If your function succeeds, return 0 and
   a negative value if it fails. */
int
__execve_environ_init(char *const envp[]) {
    (void) (envp);

    return (0);
}
