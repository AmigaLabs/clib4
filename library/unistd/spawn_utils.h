/*
 * $Id: unistd_spawn_utils.h,v 1.0 2025-01-06 11:04:00 clib4devs Exp $
*/

#ifndef _SPAWN_UTILS_H
#define _SPAWN_UTILS_H

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/*
 * Build the fd-inheritance spec string for a child process.
 * Pass fhin/fhout/fherr >= 0 to enable STDIN/STDOUT/STDERR symbolic tokens
 * (needed for PIPE: handles).  Pass -1 when no stdio redirection is in use.
 * Returns a malloc'd string, or NULL if no fds need inheritance.
 */
char *build_fd_inherit_spec(struct _clib4 *__clib4, int fhin, int fhout, int fherr);

/*
 * Close DupFileHandle'd BPTRs stored in an fd-inheritance spec, skipping
 * symbolic tokens (STDIN/STDOUT/STDERR).
 */
void close_fd_inherit_spec_handles(char *spec);

/*
 * Return a malloc'd copy of the calling process' DOS local variable "name",
 * or NULL if it has none. Used to restore the caller's local variables after
 * the spawn functions set the child's environment on them: that state must
 * be read from dos.library, not from getenv(), whose environ also holds the
 * values the program setenv()ed, or those would leak into the caller (the
 * Shell, when the program runs on the Shell's process) for good.
 */
char *get_local_var_copy(const char *name);

#endif /* _SPAWN_UTILS_H */
