/*
 * $Id: macros.h,v 1.22 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MACROS_H
#define _MACROS_H

/****************************************************************************/

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif /* DOS_DOS_H */

/****************************************************************************/
#ifdef ERROR
#undef ERROR
#endif

#define BUSY ((struct IORequest *)NULL)
#define CANNOT !
#define DO_NOTHING ((void)0)
#define NO !
#define NOT !
#define DO_NOT !
#define OK (0)
#define SAME (0)
#define SEEK_ERROR (-1)
#define CHANGE_FILE_ERROR (0)
#define GETPOSITION_ERROR (-1LL)
#define ERROR (-1)

/****************************************************************************/

#define NUM_ENTRIES(t) (sizeof(t) / sizeof(t[0]))

/****************************************************************************/

#define FLAG_IS_SET(v,f)	(((v) & (f)) == (f))
#define FLAG_IS_CLEAR(v,f)	(((v) & (f)) ==  0 )

/****************************************************************************/

#define SET_FLAG(v,f)		((void)((v) |=  (f)))
#define CLEAR_FLAG(v,f)		((void)((v) &= ~(f)))

/****************************************************************************/

/* Macro to get longword-aligned stack space for a structure
   Uses ANSI token catenation to form a name for the char array
   based on the variable name, then creates an appropriately
   typed pointer to point to the first longword boundary in the
   char array allocated. */
#define D_S(type, name) \
	char a_##name[sizeof(type)+3]; \
	type *name = (type *)((ULONG)(a_##name+3) & ~3)

/****************************************************************************/

/* BCPL style "NULL"; this should be in <dos/dos.h>. */
#ifndef BZERO
#define BZERO ((BPTR)NULL)
#endif /* BZERO */

/****************************************************************************/

/* Special data and function attributes; for OS4 most, if not all of them
   are in a file called <amiga_compiler.h> which is pulled in by the
   <exec/types.h> header file. */

/****************************************************************************/

#ifndef IsMinListEmpty
#define IsMinListEmpty(ml) \
    ((struct MinList *)((ml)->mlh_TailPred) == (struct MinList *)(ml))
#endif

#ifndef AMIGA_COMPILER_H

#ifdef __GNUC__

#ifndef ASM
#define ASM
#endif /* ASM */

#ifndef REG
#define REG(r,p) p __asm(#r)
#endif /* REG */

#ifndef INTERRUPT
#define INTERRUPT __attribute__((__interrupt__))
#endif /* INTERRUPT */

#ifndef INLINE
#define INLINE __inline__
#endif /* INLINE */

#endif /* __GNUC__ */

#endif /* AMIGA_COMPILER_H */

#ifndef WEAK
#ifdef __GNUC__
#define WEAK __attribute__((weak))
#else
#define WEAK /* WEAK */
#endif /* __GNUC__ */
#endif /* WEAK */

/****************************************************************************/

#ifndef UNUSED
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED /* UNUSED */
#endif /* __GNUC__ */
#endif /* UNUSED */

/****************************************************************************/

#ifndef USED
#ifdef __GNUC__
#define USED __attribute__((used))
#else
#define USED /* USED */
#endif /* __GNUC__ */
#endif /* USED */

/****************************************************************************/
#ifndef NOCOMMON
#ifdef __GNUC__
#define NOCOMMON __attribute__((nocommon))
#else
#define NOCOMMON /* NOCOMMON */
#endif /* __GNUC__ */
#endif /* NOCOMMON */

#ifndef HIDDEN
#ifdef __GNUC__
# define HIDDEN __attribute__ ((__visibility__ ("hidden")))
#else
# define HIDDEN
#endif /* __GNUC__ */
#endif /* HIDDEN */

/****************************************************************************/

#endif /* _MACROS_H */
