/*
 * $Id: stdlib_rand.c,v 1.6 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
rand(void)
{
	int result;

	result = rand_r(&__global_clib2->__random_seed);

	return(result);
}
