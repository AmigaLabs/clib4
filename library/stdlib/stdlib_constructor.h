/*
 * $Id: stdlib_constructor.h,v 1.3 2006-01-08 12:04:25 clib2devs Exp $
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

#define CONSTRUCTOR(name,pri) \
	STATIC VOID __attribute__((used)) name##_ctor(VOID); \
	STATIC VOID (*__##name##_ctor)(VOID) __attribute__((used,section(".ctors._" #pri))) = name##_ctor; \
	STATIC VOID name##_ctor(VOID)

#define DESTRUCTOR(name,pri) \
	STATIC VOID __attribute__((used)) name##_dtor(VOID); \
	STATIC VOID (*__##name##_dtor)(VOID) __attribute__((used,section(".dtors._" #pri))) = name##_dtor; \
	STATIC VOID name##_dtor(VOID)

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

#define STDLIB_CONSTRUCTOR(name)	CONSTRUCTOR(name,	9)
#define STDLIB_DESTRUCTOR(name)		DESTRUCTOR(name,	9)

#define STK_CONSTRUCTOR(name)		CONSTRUCTOR(name,	8)
#define STK_DESTRUCTOR(name)		DESTRUCTOR(name,	8)

#define STDIO_CONSTRUCTOR(name)		CONSTRUCTOR(name,	7)
#define STDIO_DESTRUCTOR(name)		DESTRUCTOR(name,	7)

#define FILE_CONSTRUCTOR(name)		CONSTRUCTOR(name,	6)
#define FILE_DESTRUCTOR(name)		DESTRUCTOR(name,	6)

#define MATH_CONSTRUCTOR(name)		CONSTRUCTOR(name,	5)
#define MATH_DESTRUCTOR(name)		DESTRUCTOR(name,	5)

#define SOCKET_CONSTRUCTOR(name)	CONSTRUCTOR(name,	4)
#define SOCKET_DESTRUCTOR(name)		DESTRUCTOR(name,	4)

#define ARG_CONSTRUCTOR(name)		CONSTRUCTOR(name,	3)
#define ARG_DESTRUCTOR(name)		DESTRUCTOR(name,	3)

#define CLIB_CONSTRUCTOR(name)		CONSTRUCTOR(name,	2)
#define CLIB_DESTRUCTOR(name)		DESTRUCTOR(name,	2)

#define PROFILE_CONSTRUCTOR(name)	CONSTRUCTOR(name,	1)
#define PROFILE_DESTRUCTOR(name)	DESTRUCTOR(name,	1)

/****************************************************************************/

#endif /* _STDLIB_CONSTRUCTOR_H */
