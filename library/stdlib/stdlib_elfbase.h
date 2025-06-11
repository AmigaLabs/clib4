/*
 * $Id: stdlib_elfbase.h,v 1.0 2025-04-15 10:06:25 clib4devs Exp $
*/

#ifndef _STDLIB_ELFBASE_H
#define _STDLIB_ELFBASE_H

#ifndef __NOLIBBASE__
#define __NOLIBBASE__
#endif /* __NOLIBBASE__ */

#ifndef __NOGLOBALIFACE__
#define __NOGLOBALIFACE__
#endif /* __NOGLOBALIFACE__ */

#include <proto/elf.h>

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#define DECLARE_ELFBASE() \
	struct ElfIFace *IElf = __CLIB4->IElf;

#define DECLARE_ELFBASE_R(clib4) \
	struct ElfIFace *IElf = clib4->IElf;

#endif /* _STDLIB_ELFBASE_H */
