/* $Id: socket_res_state.c,v 1.0 2022-03-14 10:44:15 clib4devs Exp $
 */

#include <dos.h>
#include <resolv.h>

/* This is completely unused, and exists purely to satisfy broken apps. */
struct __res_state *__res_state() {
    struct _clib4 *__clib4 = __CLIB4;
    return &__clib4->_res_state;
}