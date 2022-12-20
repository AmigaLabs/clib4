/*
 * $Id: 
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
vdprintf(int fd, const char *format, va_list ap) {
    int ret;
    char buf[512];
    char *ptr = &buf[0]; /* prefer stack, but use heap if we need */

    ENTER();
    SHOWVALUE(fd);
    SHOWSTRING(format);

    ret = vsnprintf(ptr, 512, format, ap);
    if(ret >= 512)
    {
	int ret2;
	ptr = (char*)malloc(ret+1);
	assert(ptr);
	ret2 = vsnprintf(ptr, ret, format, ap);
	assert(ret2 < ret);
	ret=ret2;
    }
    
    ret = write(fd,ptr,ret);

    if(ptr != &buf[0])
      free(ptr);
    
    RETURN(ret);
    return ret;
}
