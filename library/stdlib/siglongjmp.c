/*
 * $Id: stdlib_siglongjmp.c,v 1.0 2023-02-28 14:12:15 clib2devs Exp $
*/

#include <setjmp.h>

void siglongjmp(sigjmp_buf buf, int ret) {
    longjmp(buf->__jmpbuf, ret);
}