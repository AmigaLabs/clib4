/*
 * $Id: stdlib_sysbase.c,v 1.5 2006-01-08 12:04:26 clib2devs Exp $
*/

#define __NOLIBBASE__

#ifndef PROTO_EXEC_H
#include <proto/exec.h>
#endif /* PROTO_EXEC_H */

#include "macros.h"

struct ExecBase  *SysBase;
struct ExecIFace *IExec;