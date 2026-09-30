/*
 * wordexp.h for clib4
 *
 * POSIX.2 wordexp/wordfree interface.
 */

#ifndef _WORDEXP_H
#define _WORDEXP_H

#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

/* Flags for wordexp() */
enum {
    WRDE_DOOFFS  = (1 << 0),   /* Insert PWORDEXP->we_offs NULLs.  */
    WRDE_APPEND  = (1 << 1),   /* Append to results of a previous call.  */
    WRDE_NOCMD   = (1 << 2),   /* Don't do command substitution.  */
    WRDE_REUSE   = (1 << 3),   /* Reuse storage in PWORDEXP.  */
    WRDE_SHOWERR = (1 << 4),   /* Don't redirect stderr to /dev/null.  */
    WRDE_UNDEF   = (1 << 5),   /* Error for expanding undefined variables.  */
    __WRDE_FLAGS = (WRDE_DOOFFS | WRDE_APPEND | WRDE_NOCMD |
                    WRDE_REUSE | WRDE_SHOWERR | WRDE_UNDEF)
};

typedef struct {
    size_t we_wordc;       /* Count of words matched.  */
    char **we_wordv;       /* List of expanded words.  */
    size_t we_offs;        /* Slots to reserve in we_wordv.  */
} wordexp_t;

/* Return values from wordexp() */
enum {
    WRDE_NOSYS   = -1,     /* Never used since we support `wordexp'.  */
    WRDE_NOSPACE = 1,      /* Ran out of memory.  */
    WRDE_BADCHAR,          /* A metachar appears in the wrong place.  */
    WRDE_BADVAL,           /* Undefined var reference with WRDE_UNDEF.  */
    WRDE_CMDSUB,           /* Command substitution with WRDE_NOCMD.  */
    WRDE_SYNTAX            /* Shell syntax error.  */
};

/* Expand WORDS like a shell and fill in PWORDEXP.  */
extern int wordexp(const char *__restrict __words,
                   wordexp_t *__restrict __pwordexp, int __flags);

/* Free the storage allocated in PWORDEXP by wordexp().  */
extern void wordfree(wordexp_t *__pwordexp);

__END_DECLS

#endif /* _WORDEXP_H */
