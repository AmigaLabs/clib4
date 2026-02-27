/*
* $Id: stdlib_canonicalize_file_name.c,v 1.0 2025-06-27 12:59:00 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

char *
canonicalize_file_name(const char *name) {
	return realpath(name, NULL);
}
