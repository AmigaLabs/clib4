/*
 * $Id: stdnoreturn.h,v 1.0 2022-03-22 13:12:59 clib4devs Exp $
*/

#ifndef _STDNORETURN_H
#define _STDNORETURN_H

#include <features.h>

#ifndef __cplusplus

#if __STDC_VERSION__ >= 200112L

#define noreturn _Noreturn

#endif /* __STDC_VERSION__ >= 201112L */
#endif /* __cplusplus */
#endif /* _STD_NORETURN */
