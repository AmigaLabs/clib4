/*
 * $Id: stdlib_constructor.h,v 1.4 2023-02-11 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_CONSTRUCTOR_H
#define _STDLIB_CONSTRUCTOR_H

/****************************************************************************/

/* Constructor and destructor functions, as used by the library for data
   initialization and cleanup. These particular functions are invoked by
   the startup code before and after the main() function is/was called.
   How this works is very compiler specific. We support three flavours
   below. */

/****************************************************************************/

#define CONSTRUCTOR(name, pri) \
    static void __attribute__((used, constructor(pri))) name##_ctor(VOID); \
    static void (*__##name##_ctor)(void) = name##_ctor; \
    static void name##_ctor(void)

#define DESTRUCTOR(name, pri) \
    static void __attribute__((used, destructor(pri))) name##_dtor(VOID); \
    static void (*__##name##_dtor)(void) = name##_dtor; \
    static void name##_dtor(void)

#define CONSTRUCTOR_SUCCEED() \
	return

#define CONSTRUCTOR_FAIL() \
	exit(RETURN_FAIL)

/****************************************************************************/

/* These macros are for declaring functions to serve as constructors or
   destructors. In which order these should be invoked is defined by the
   priority, which is a number in the range 0-999. User-supplied
   constructor/destructor functions should have priority 0. That way,
   the user-supplied constructors will be invoked after the library
   constructors and the user-supplied destructors before the library
   destructors. */

#define STDLIB_CONSTRUCTOR(name)	CONSTRUCTOR(name,	1)
#define STDLIB_DESTRUCTOR(name)		DESTRUCTOR(name,	1)

#define STK_CONSTRUCTOR(name)		CONSTRUCTOR(name,	2)
#define STK_DESTRUCTOR(name)		DESTRUCTOR(name,	2)

#define STDIO_CONSTRUCTOR(name)		CONSTRUCTOR(name,	3)
#define STDIO_DESTRUCTOR(name)		DESTRUCTOR(name,	3)

#define FILE_CONSTRUCTOR(name)		CONSTRUCTOR(name,	4)
#define FILE_DESTRUCTOR(name)		DESTRUCTOR(name,	4)

#define MATH_CONSTRUCTOR(name)		CONSTRUCTOR(name,	5)
#define MATH_DESTRUCTOR(name)		DESTRUCTOR(name,	5)

#define SOCKET_CONSTRUCTOR(name)	CONSTRUCTOR(name,	6)
#define SOCKET_DESTRUCTOR(name)		DESTRUCTOR(name,	6)

#define ARG_CONSTRUCTOR(name)		CONSTRUCTOR(name,	7)
#define ARG_DESTRUCTOR(name)		DESTRUCTOR(name,	7)

#define CLIB_CONSTRUCTOR(name)		CONSTRUCTOR(name,	8)
#define CLIB_DESTRUCTOR(name)		DESTRUCTOR(name,	8)

#define PROFILE_CONSTRUCTOR(name)	CONSTRUCTOR(name,	9)
#define PROFILE_DESTRUCTOR(name)	DESTRUCTOR(name,	9)

/****************************************************************************/

#endif /* _STDLIB_CONSTRUCTOR_H */
