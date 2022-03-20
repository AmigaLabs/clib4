/*
 * $Id: stdlib_utilitybase.c,v 1.3 2006-01-08 12:04:26 clib2devs Exp $
*/

#define __NOLIBBASE__

#ifndef PROTO_UTILITY_H
#include <proto/utility.h>
#endif /* PROTO_UTILITY_H */

/****************************************************************************/

#include "macros.h"

/****************************************************************************/

struct Library      NOCOMMON *__UtilityBase;
struct UtilityIFace NOCOMMON *__IUtility;
