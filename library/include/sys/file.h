#ifndef __SYS_FILE_H__
#define __SYS_FILE_H__

#include <features.h>

__BEGIN_DECLS

#define LOCK_SH	1
#define LOCK_EX	2
#define LOCK_NB	4
#define LOCK_UN	8

#define L_SET 0
#define L_INCR 1
#define L_XTND 2

extern int flock(int fd, int op);

__END_DECLS

#endif