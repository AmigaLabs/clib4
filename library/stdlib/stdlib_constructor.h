/*
 * $Id: stdlib_constructor.h,v 1.4 2023-02-11 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_CONSTRUCTOR_H
#define _STDLIB_CONSTRUCTOR_H

#define NEW_CONSTRUCTOR_CALL

/****************************************************************************/

/* Constructor and destructor functions, as used by the library for data
   initialization and cleanup. These particular functions are invoked by
   the startup code before and after the main() function is/was called.
   How this works is very compiler specific. We support three flavours
   below. */

/****************************************************************************/

#ifdef NEW_CONSTRUCTOR_CALL
#define CONSTRUCTOR(name, pri) \
    static void __attribute__((used, constructor(pri))) name##_ctor(VOID); \
    static void (*__##name##_ctor)(void) = name##_ctor; \
    static void name##_ctor(void)

#define DESTRUCTOR(name, pri) \
    static void __attribute__((used, destructor(pri))) name##_dtor(VOID); \
    static void (*__##name##_dtor)(void) = name##_dtor; \
    static void name##_dtor(void)

#define NSCONSTRUCTOR(name, pri) \
    void __attribute__((used, constructor(pri))) name##_ctor(VOID); \
    void (*__##name##_ctor)(void) = name##_ctor; \
    void name##_ctor(void)

#define NSDESTRUCTOR(name, pri) \
    void __attribute__((used, destructor(pri))) name##_dtor(VOID); \
    void (*__##name##_dtor)(void) = name##_dtor; \
    void name##_dtor(void)

#else

#define CONSTRUCTOR(name,pri) \
	STATIC VOID __attribute__((used)) name##_ctor(VOID); \
	STATIC VOID (*__##name##_ctor)(VOID) __attribute__((used,section(".ctors._" #pri))) = name##_ctor; \
	STATIC VOID name##_ctor(VOID)

#define DESTRUCTOR(name,pri) \
	STATIC VOID __attribute__((used)) name##_dtor(VOID); \
	STATIC VOID (*__##name##_dtor)(VOID) __attribute__((used,section(".dtors._" #pri))) = name##_dtor; \
	STATIC VOID name##_dtor(VOID)
#endif

#define CONSTRUCTOR_SUCCEED() \
	return

// This can cause problems on libraries because of exit. We should find a better way to do this
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

#ifdef NEW_CONSTRUCTOR_CALL
#define STDLIB_CONSTRUCTOR(name)	CONSTRUCTOR(name,	101)
#define STDLIB_DESTRUCTOR(name)		DESTRUCTOR(name,	101)

#define STK_CONSTRUCTOR(name)		CONSTRUCTOR(name,	102)
#define STK_DESTRUCTOR(name)		DESTRUCTOR(name,	102)

#define STDIO_CONSTRUCTOR(name)		CONSTRUCTOR(name,	103)
#define STDIO_DESTRUCTOR(name)		DESTRUCTOR(name,	103)

#define FILE_CONSTRUCTOR(name)		CONSTRUCTOR(name,	104)
#define FILE_DESTRUCTOR(name)		DESTRUCTOR(name,	104)

#define MATH_CONSTRUCTOR(name)		CONSTRUCTOR(name,	105)
#define MATH_DESTRUCTOR(name)		DESTRUCTOR(name,	105)

#define SOCKET_CONSTRUCTOR(name)	CONSTRUCTOR(name,	106)
#define SOCKET_DESTRUCTOR(name)		DESTRUCTOR(name,	106)

#define ARG_CONSTRUCTOR(name)		CONSTRUCTOR(name,	107)
#define ARG_DESTRUCTOR(name)		DESTRUCTOR(name,	107)

#define CLIB_CONSTRUCTOR(name)		CONSTRUCTOR(name,	108)
#define CLIB_DESTRUCTOR(name)		DESTRUCTOR(name,	108)

#define PTHREAD_CONSTRUCTOR(name)	NSCONSTRUCTOR(name,	108)
#define PTHREAD_DESTRUCTOR(name)	NSDESTRUCTOR(name,	108)

#define PROFILE_CONSTRUCTOR(name)	CONSTRUCTOR(name,	109)
#define PROFILE_DESTRUCTOR(name)	DESTRUCTOR(name,	109)

#else
#define STDLIB_CONSTRUCTOR(name)	CONSTRUCTOR(name,	10)
#define STDLIB_DESTRUCTOR(name)		DESTRUCTOR(name,	10)

#define STK_CONSTRUCTOR(name)		CONSTRUCTOR(name,	9)
#define STK_DESTRUCTOR(name)		DESTRUCTOR(name,	9)

#define STDIO_CONSTRUCTOR(name)		CONSTRUCTOR(name,	8)
#define STDIO_DESTRUCTOR(name)		DESTRUCTOR(name,	8)

#define FILE_CONSTRUCTOR(name)		CONSTRUCTOR(name,	7)
#define FILE_DESTRUCTOR(name)		DESTRUCTOR(name,	7)

#define MATH_CONSTRUCTOR(name)		CONSTRUCTOR(name,	6)
#define MATH_DESTRUCTOR(name)		DESTRUCTOR(name,	6)

#define SOCKET_CONSTRUCTOR(name)	CONSTRUCTOR(name,	5)
#define SOCKET_DESTRUCTOR(name)		DESTRUCTOR(name,	5)

#define ARG_CONSTRUCTOR(name)		CONSTRUCTOR(name,	4)
#define ARG_DESTRUCTOR(name)		DESTRUCTOR(name,	4)

#define CLIB_CONSTRUCTOR(name)		CONSTRUCTOR(name,	3)
#define CLIB_DESTRUCTOR(name)		DESTRUCTOR(name,	3)

#define PTHREAD_CONSTRUCTOR(name)	CONSTRUCTOR(name,	2)
#define PTHREAD_DESTRUCTOR(name)	DESTRUCTOR(name,	2)

#define PROFILE_CONSTRUCTOR(name)	CONSTRUCTOR(name,	1)
#define PROFILE_DESTRUCTOR(name)	DESTRUCTOR(name,	1)
#endif


/****************************************************************************/

#endif /* _STDLIB_CONSTRUCTOR_H */
