/*
 * $Id: stdalign.h ,v1.0 2022-03-22 13:12:59 clib4devs Exp $
 */

#ifndef _STDALIGN_H
#define _STDALIGN_H

#include <features.h>

#ifndef __cplusplus

#if __STDC_VERSION__ < 201112L

#if defined __has_attribute
#if __has_attribute(__aligned__)
#define _Alignas(t) __attribute__((__aligned__(t)))
#endif /* __has_attribute(__aligned__) */
#if __has_attribute(__alignof__)
#define _Alignof(t) __alignof__(t)
#endif /* __has_attribute(__alignof__) */
#endif /* __has_attribute */

#endif /* __STDC_VERSION__ < 201112L */

#define alignas _Alignas
#define alignof _Alignof

#endif /* __cplusplus */

#define __alignas_is_defined 1
#define __alignof_is_defined 1

#endif /* _STDALIGN_H */
