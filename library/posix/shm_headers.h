/*
 * $Id: shm_headers.h,v 1.1 2023-06-03 12:06:14 clib2devs Exp $
*/

#ifndef _SHM_HEADERS_H
#define _SHM_HEADERS_H

#define _TIMESPEC_DEFINED

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <sys/sem.h>

#ifndef _STDLIB_SYSVBASE_H
#include "stdlib_sysvbase.h"
#endif /* _STDLIB_SYSVBASE_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

extern struct ExecIFace *IExec;

#define SHMFLG_DeleteMe (1L << 0)

#endif