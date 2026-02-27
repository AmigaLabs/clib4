/*
 * $Id: debug.h,v 1.5 2024-08-17 12:04:22 clib4devs Exp $
 */

#ifndef _DEBUG_H
#define _DEBUG_H

/* IMPORTANT: If DEBUG is redefined, it must happen only here. This
 *            will cause all modules to depend upon it to be rebuilt
 *            by the smakefile (that is, provided the smakefile has
 *            all the necessary dependency lines in place).
 */

/*#define DEBUG*/

#define DEBUGLEVEL_OnlyAsserts  0
#define DEBUGLEVEL_Reports      1
#define DEBUGLEVEL_CallTracing  2

#define PUSH_REPORTS()	PUSHDEBUGLEVEL(1)
#define PUSH_CALLS()	PUSHDEBUGLEVEL(2)
#define PUSH_ALL()	PUSHDEBUGLEVEL(2)
#define POP()		POPDEBUGLEVEL()

#if defined(DEBUG)
    #ifndef _STDLIB_H
    #include <stdlib.h>
    #endif /* _STDLIB_H */

    #ifndef _DOS_H
    #include <dos.h>
    #endif /* _DOS_H */

    void kprintf(const char *format,...);

    void _SHOWVALUE(unsigned long value,int size,const char *name,const char *file,int line);
    void _SHOWPOINTER(const void *p,const char *name,const char *file,int line);
    void _SHOWSTRING(const char *string,const char *name,const char *file,int line);
    void _SHOWWSTRING(const wchar_t *string,const char *name,const char *file,int line);
    void _SHOWMSG(const char *msg,const char *file,int line);
    void _ENTER(const char *file,int line,const char *function);
    void _LEAVE(const char *file,int line,const char *function);
    void _RETURN(const char *file,int line,const char *function,unsigned long result);
    void _DPRINTF_HEADER(const char *file,int line);
    void _DPRINTF(const char *format,...);
    void _DLOG(const char *format,...);
    int  _SETDEBUGLEVEL(int level);
    int  _GETDEBUGLEVEL(void);
    void _SETPROGRAMNAME(char *name);

    #define ENTER()		        _ENTER(__FILE__, __LINE__, __FUNCTION__)
    #define LEAVE()		        _LEAVE(__FILE__, __LINE__, __FUNCTION__)
    #define RETURN(r)		    _RETURN(__FILE__,__LINE__,__FUNCTION__,(unsigned long)r)
    #define SHOWVALUE(v)		_SHOWVALUE((unsigned long)v,sizeof(v),#v,__FILE__,__LINE__)
    #define SHOWPOINTER(p)	    _SHOWPOINTER(p,#p,__FILE__,__LINE__)
    #define SHOWSTRING(s)		_SHOWSTRING(s,#s,__FILE__,__LINE__)
    #define SHOWWSTRING(s)		_SHOWWSTRING(s,#s,__FILE__,__LINE__)
    #define SHOWMSG(s)		    _SHOWMSG(s,__FILE__,__LINE__)
    #define D(s)			    do { _DPRINTF_HEADER(__FILE__,__LINE__); _DPRINTF s; } while(0)
    #define PRINTHEADER()		_DPRINTF_HEADER(__FILE__,__LINE__)
    #define LOG(s)		        do { _DPRINTF_HEADER(__FILE__,__LINE__); _DLOG("<%s()>:",__FUNCTION__); _DLOG s; } while(0)
    #define SETDEBUGLEVEL(l)	_SETDEBUGLEVEL(l)
    #define GETDEBUGLEVEL()	    _GETDEBUGLEVEL()

    #undef DEBUG
    #define DEBUG 1

    #undef assert
    #define assert(expression)                     \
    ((void)                                \
     ((expression) ? 0 :                   \
      (                                    \
       _SETDEBUGLEVEL(2),                  \
       kprintf("[%s] "                     \
        "%s:%ld: failed assertion '%s'\n", \
        __CLIB4->__progname,               \
        __FILE__,                          \
        __LINE__,                          \
        #expression),                      \
       abort(),                            \
       0                                   \
      )                                    \
     )                                     \
    )

#else

    #ifdef NDEBUG
        #define assert(expression) ((void)0)
    #else
        #include <assert.h>
    #endif /* NDEBUG */


    #define ENTER()		        ((void)0)
    #define LEAVE()		        ((void)0)
    #define RETURN(r)		    ((void)0)
    #define SHOWVALUE(v)		((void)0)
    #define SHOWPOINTER(p)		((void)0)
    #define SHOWSTRING(s)		((void)0)
    #define SHOWWSTRING(s)		((void)0)
    #define SHOWMSG(s)		    ((void)0)
    #define D(s)			    ((void)0)
    #define PRINTHEADER()		((void)0)
    #define LOG(s)			    ((void)0)
    #define SETDEBUGLEVEL(l)	((void)0)
    #define PUSHDEBUGLEVEL(l)	((void)0)
    #define POPDEBUGLEVEL()	    ((void)0)
    #define GETDEBUGLEVEL()	    (0)

    #define DEBUG 0

#endif /* DEBUG */


#endif /* _DEBUG_H */
