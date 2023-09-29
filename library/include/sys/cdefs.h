#ifndef _SYS_CDEFS_H
#define _SYS_CDEFS_H

#define __FBSDID(x) /* nothing */
/*
 * Note: the goal here is not compatibility to K&R C. Since we know that we
 * have GCC which understands ANSI C perfectly well, we make use of this.
 */

#define __P(args) args
#define __PMT(args) args
#define __const const
#define __signed signed
#define __volatile volatile
#define __DOTS , ...
#define __THROW

#define __ptr_t void *

#define __attribute_malloc__
#define __attribute_pure__
#define __attribute_format_strfmon__(a, b)
#define __flexarr [0]

#ifdef __cplusplus
#define __BEGIN_DECLS \
	extern "C"        \
	{
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#ifndef __BOUNDED_POINTERS__
#define __bounded	/* nothing */
#define __unbounded /* nothing */
#define __ptrvalue	/* nothing */
#endif

#ifdef __GNUC__
#define __strong_reference(sym, aliassym) \
	extern __typeof(sym) aliassym __attribute__((__alias__(#sym)));
#ifdef __ELF__
#ifdef __STDC__
#define __weak_reference(sym, alias) \
	__asm__(".weak " #alias);        \
	__asm__(".equ " #alias ", " #sym)
#define __warn_references(sym, msg)   \
	__asm__(".section .gnu.warning"); \
	__asm__(".asciz \"" msg "\"");    \
	__asm__(".previous")
#else
#define __weak_reference(sym, alias) \
	__asm__(".weak alias");          \
	__asm__(".equ alias, sym")
#define __warn_references(sym, msg)   \
	__asm__(".section .gnu.warning"); \
	__asm__(".asciz \"msg\"");        \
	__asm__(".previous")
#endif /* __STDC__ */
#else  /* !__ELF__ */
#ifdef __STDC__
#define __weak_reference(sym, alias)            \
	__asm__(".stabs \"_" #alias "\",11,0,0,0"); \
	__asm__(".stabs \"_" #sym "\",1,0,0,0")
#define __warn_references(sym, msg)         \
	__asm__(".stabs \"" msg "\",30,0,0,0"); \
	__asm__(".stabs \"_" #sym "\",1,0,0,0")
#else
#define __weak_reference(sym, alias)           \
	__asm__(".stabs \"_/**/alias\",11,0,0,0"); \
	__asm__(".stabs \"_/**/sym\",1,0,0,0")
#define __warn_references(sym, msg) \
	__asm__(".stabs msg,30,0,0,0"); \
	__asm__(".stabs \"_/**/sym\",1,0,0,0")
#endif /* __STDC__ */
#endif /* __ELF__ */

#ifdef __has_builtin
# define __clib4_has_builtin(name) __has_builtin (name)
#else
# define __clib4_has_builtin(name) 0
#endif

#if (__GNUC__ >= 3) || __clib4_has_builtin (__builtin_expect)
# define __clib4_unlikely(cond)	__builtin_expect ((cond), 0)
# define __clib4_likely(cond)	__builtin_expect ((cond), 1)
#else
# define __clib4_unlikely(cond)	(cond)
# define __clib4_likely(cond)	(cond)
#endif

#endif /* __GNUC__ */

#endif /* _SYS_CDEFS_H */
