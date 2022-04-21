/*
 * $Id: wchar_fwide.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int 
fwide(FILE *fp, int mode)
{
	int ret;

    ENTER();

	flockfile(fp);
	if (mode != 0)
	{
		ORIENT(fp, mode);
	}
	if (!(fp->_flags & __SORD))
		ret = 0;
	else
		ret = (fp->_flags2 & __SWID) ? 1 : -1;
	funlockfile(fp);
	return ret;
}
