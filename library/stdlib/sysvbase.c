/*
 * $Id: stdlib_sysvbase.c,v 1.0 2021-01-27 23:41:26 clib2devs Exp $
*/

#define __NOLIBBASE__

#include <sys/ipc.h>
#include <sys/shm.h>

#ifndef PROTO_SYSVIPC_H
#define _TIMESPEC_DEFINED
#include <posix/shm_headers.h>
#include "proto/sysvipc.h"
#endif /* PROTO_SYSVIPC_H */

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

struct Library   *__SysVBase;
struct SYSVIFace *__ISysVIPC;
