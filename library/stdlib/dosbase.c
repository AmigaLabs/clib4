/*
 * $Id: stdlib_dosbase.c,v 1.4 2006-01-08 12:04:25 clib2devs Exp $
*/

#define __NOLIBBASE__

#ifndef PROTO_DOS_H
#include <proto/dos.h>
#endif /* PROTO_DOS_H */

struct Library * DOSBase;
struct DOSIFace * IDOS;
