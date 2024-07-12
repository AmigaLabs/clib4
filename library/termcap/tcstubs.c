/*
 * $Id: termcap_tcstubs.c,v 1.0 2024-07-12 18:26:47 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <termcap.h>

char __get_tc_pc() {
    struct _clib4 *__clib4 = __CLIB4;
    return __clib4->__PC;
}

short __get_tc_ospeed() {
    struct _clib4 *__clib4 = __CLIB4;
    return __clib4->__ospeed;
}

char *__get_tc_up() {
    struct _clib4 *__clib4 = __CLIB4;
    return __clib4->__UP;
}

char *__get_tc_bc() {
    struct _clib4 *__clib4 = __CLIB4;
    return __clib4->__BC;
}