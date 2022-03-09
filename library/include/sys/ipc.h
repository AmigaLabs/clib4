#ifndef _IPC_H
#define _IPC_H

#include <sys/types.h>
#include <stdint.h>
#include <features.h>

__BEGIN_DECLS

typedef uint32_t key_t;

extern key_t ftok(const char *path, int id);

__END_DECLS

#endif