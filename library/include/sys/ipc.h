#ifndef _IPC_H
#define _IPC_H

#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint32_t key_t;

extern key_t ftok(const char *path, int id);

#ifdef __cplusplus
}
#endif


#endif