#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

#define WNOHANG 1
// #define WUNTRACED 2 Not implemented

// These are ok (and amiga specific):
#define WIFEXITED(w)	(((w) & 0x10000000) == 0)
#define WEXITSTATUS(w)	(((w)) & 0x000000ff)

// These are not
#define WIFSIGNALED(w)	(0) //(((w) & 0x7f) > 0 && (((w) & 0x7f) < 0x7f))
#define WIFSTOPPED(w)	(0) //(((w) & 0xff) == 0x7f)
#define WTERMSIG(w)	    (0) //((w) & 0x7f)
#define WSTOPSIG	WEXITSTATUS

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);

__END_DECLS

#endif