#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

#define WNOHANG 1
// #define WUNTRACED 2 Not implemented


#define WIFEXITED(w)	(((w) & 0x10000000) == 0)
#define WIFSTOPPED(w)	(((w) & 0xff) == 0x7f)
/* Note: WEXITSTATUS, WTERMSIG and WIFSIGNALED has no use on AmigaOS4 at moment */
#define WEXITSTATUS(w)	(((w)) & 0x000000ff)
#define WTERMSIG(w)	((w) & 0x7f)
#define WIFSIGNALED(w)	(((w) & 0x7f) > 0 && (((w) & 0x7f) < 0x7f))
#define WSTOPSIG	WEXITSTATUS

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);

__END_DECLS

#endif