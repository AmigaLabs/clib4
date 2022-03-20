/*
 * $Id: dirent_alphasort.c,v 1.0 2019-03-10 10:59:00 clib2devs Exp $
*/

#include <dirent.h>
#include <string.h>

#include "debug.h"

int
alphasort (const struct dirent **a, const struct dirent **b) {
	ENTER();

	int result = strcoll ((*a)->d_name, (*b)->d_name);
	
	RETURN(result);
	return(result);
}
