#ifndef _SHM_HEADERS_H
#define _SHM_HEADERS_H

#define _TIMESPEC_DEFINED

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define __USE_GNU
#include <sys/sem.h>

#ifndef _STDLIB_SYSVBASE_H
#include "stdlib_sysvbase.h"
#endif /* _STDLIB_SYSVBASE_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#endif