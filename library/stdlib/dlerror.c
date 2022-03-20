/*
 * $Id: stdlib_dlerror.c,v 1.2 2010-08-21 11:37:03 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <dlfcn.h>
#include <libraries/elf.h>

const char * dlerror(void)
{
	const char * result;

	switch(__global_clib2->__elf_error_code)
	{
		case ELF32_NO_ERROR:

			result = NULL;
			break;

		case ELF32_OUT_OF_MEMORY:

			result = "out of memory";
			break;

		case ELF32_INVALID_HANDLE:

			result = "invalid handle";
			break;

		case ELF32_NO_MORE_RELOCS:

			result = "no more relocs";
			break;

		case ELF32_SECTION_NOT_LOADED:

			result = "section not loaded";
			break;

		case ELF32_UNKNOWN_RELOC:

			result = "unknown reloc";
			break;

		case ELF32_READ_ERROR:

			result = "read error";
			break;

		case ELF32_INVALID_SDA_BASE:

			result = "invalid SDA base";
			break;

		case ELF32_SYMBOL_NOT_FOUND:

			result = "symbol not found";
			break;

		case ELF32_INVALID_NAME:

			result = "invalid name";
			break;

		case ELF32_REQUIRED_OBJECT_MISSING:

			result = "required object missing";
			break;

		default:

			result = "unknown error";
			break;
	}

	/* Calling dlerror() will clear the error code. */
	if (__global_clib2 != NULL)
		__global_clib2->__elf_error_code = ELF32_NO_ERROR;

	return(result);
}
