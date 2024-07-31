/*
 * $Id: termcap_tcstubs.c,v 1.0 2024-07-12 18:26:47 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <termcap.h>

char *__get_tc_pc() {
    return &__CLIB4->__PC;
}

short *__get_tc_ospeed() {
    return &__CLIB4->__ospeed;
}

char **__get_tc_up() {
    return &__CLIB4->__UP;
}

char **__get_tc_bc() {
    return &__CLIB4->__BC;
}