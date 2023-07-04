/*
 * $Id: setjmp_longjmp.S,v 1.0 2023-03-02 12:04:24 clib2devs Exp $
*/

#define __NO_VMX__
#undef JB_SIZE
#undef __longjmp_symbol
#define __longjmp_symbol  _longjmp
#include "__longjmp-common.S"