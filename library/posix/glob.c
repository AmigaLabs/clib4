/*
 * $Id: glob_glob.c,v 1.0 2021-01-22 11:56:47 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <pwd.h>
#include <glob.h>

/**********************************************************************/

#define STAT stat

#define DOLLAR '$'
#define DOT '.'
#define EOS '\0'
#define LBRACKET '['
#undef NOT
#define NOT '!'
#define QUESTION '?'
#define QUOTE '\\'
#define RANGE '-'
#define RBRACKET ']'
#define SEP '/'
#define STAR '*'
#define TILDE '~'
#define UNDERSCORE '_'
#define LBRACE '{'
#define RBRACE '}'
#define SLASH '/'
#define COMMA ','

#if !defined(DEBUG) && !defined(USE_8BIT_CHAR)

#define M_QUOTE 0x8000
#define M_PROTECT 0x4000
#define M_MASK 0xffff
#define M_ASCII 0x00ff

typedef u_short char;

#else

#define M_QUOTE 0x80
#define M_PROTECT 0x40
#define M_MASK 0xff
#define M_ASCII 0x7f

#endif

#define CHAR(c) ((char)((c)&M_ASCII))
#define META(c) ((char)((c) | M_QUOTE))
#define M_ALL META('*')
#define M_END META(']')
#define M_NOT META('!')
#define M_ONE META('?')
#define M_RNG META('-')
#define M_SET META('[')
#define ismeta(c) (((c)&M_QUOTE) != 0)

/**********************************************************************
 * Static functions used in glob()                                  
 **********************************************************************/
static int glob0(const char *pattern, glob_t *pglob);
static int globexp1(const char *pattern, glob_t *pglob);
static int globexp2(const char *ptr, const char *pattern, glob_t *pglob, int *rv);
static const char *globtilde(const char *pattern, char *patbuf, size_t patsize, glob_t *pglob);
static int compare(const void *p, const void *q);
static int glob1(char *pattern, glob_t *pglob, size_t *limit);
static int glob2(char *pathbuf, char *pathend, char *pathlim, char *pattern, glob_t *pglob, size_t *limit);
static int glob3(char *pathbuf, char *pathend, char *pathlim, char *pattern, char *restpattern, glob_t *pglob, size_t *limit);
static DIR *g_opendir(char *str, glob_t *pglob);
static int g_lstat(char *fn, struct STAT *sb, glob_t *pglob);
static int g_stat(char *fn, struct STAT *sb, glob_t *pglob);
static char *g_strchr(const char *str, int ch);
static int g_Ctoc(const char *str, char *buf, size_t len);
static int globextend(const char *path, glob_t *pglob, size_t *limit);
static int match(char *name, char *pat, char *patend);

/**********************************************************************/

static DIR *
g_opendir(char *str, glob_t *pglob)
{
    char buf[MAXPATHLEN] = {0};

    if (!*str)
        (void)strlcpy(buf, ".", sizeof(buf));
    else
    {
        if (g_Ctoc(str, buf, sizeof(buf)))
            return NULL;
    }
    if (pglob->gl_flags & GLOB_ALTDIRFUNC)
        return ((*pglob->gl_opendir)(buf));

    return (opendir(buf));
}

static int
g_lstat(char *fn, struct STAT *sb, glob_t *pglob)
{
    char buf[MAXPATHLEN] = {0};

    if (g_Ctoc(fn, buf, sizeof(buf)))
        return -1;
    if (pglob->gl_flags & GLOB_ALTDIRFUNC)
        return ((*pglob->gl_lstat)(buf, sb));
    return (lstat(buf, sb));
}

static int
g_stat(char *fn, struct STAT *sb, glob_t *pglob)
{
    char buf[MAXPATHLEN] = {0};

    if (g_Ctoc(fn, buf, sizeof(buf)))
        return -1;
    if (pglob->gl_flags & GLOB_ALTDIRFUNC)
        return ((*pglob->gl_stat)(buf, sb));
    return (stat(buf, sb));
}

static char *
g_strchr(const char *str, int ch)
{
    do
    {
        if (*str == ch)
            /* LINTED this is libc's definition! */
            return (char *)str;
    } while (*str++);
    return NULL;
}

static int
g_Ctoc(const char *str, char *buf, size_t len)
{
    char *dc;

    if (len == 0)
        return 1;

    for (dc = buf; len && (*dc++ = *str++) != EOS; len--)
        continue;

    return len == 0;
}

static int
match(char *name, char *pat, char *patend)
{
    int ok, negate_range;
    char c, k;

    while (pat < patend)
    {
        c = *pat++;
        switch (c & M_MASK)
        {
        case M_ALL:
            if (pat == patend)
                return (1);
            do
                if (match(name, pat, patend))
                    return (1);
            while (*name++ != EOS);
            return (0);
        case M_ONE:
            if (*name++ == EOS)
                return (0);
            break;
        case M_SET:
            ok = 0;
            if ((k = *name++) == EOS)
                return (0);
            if ((negate_range = ((*pat & M_MASK) == M_NOT)) != EOS)
                ++pat;
            while (((c = *pat++) & M_MASK) != M_END)
                if ((*pat & M_MASK) == M_RNG)
                {
                    if (c <= k && k <= pat[1])
                        ok = 1;
                    pat += 2;
                }
                else if (c == k)
                    ok = 1;
            if (ok == negate_range)
                return (0);
            break;
        default:
            if (*name++ != c)
                return (0);
            break;
        }
    }
    return (*name == EOS);
}

static int
globextend(const char *path, glob_t *pglob, size_t *limit)
{
    char **pathv;
    int i;
    size_t newsize, len;
    char *copy;
    const char *p;

    newsize = sizeof(*pathv) * (2 + pglob->gl_pathc + pglob->gl_offs);
    pathv = pglob->gl_pathv ? realloc(pglob->gl_pathv, newsize) : malloc(newsize);
    if (pathv == NULL)
        return (GLOB_NOSPACE);

    if (pglob->gl_pathv == NULL && pglob->gl_offs > 0)
    {
        /* first time around -- clear initial gl_offs items */
        pathv += pglob->gl_offs;
        for (i = pglob->gl_offs; --i >= 0;)
            *--pathv = NULL;
    }
    pglob->gl_pathv = pathv;

    for (p = path; *p++;)
        continue;
    len = (size_t)(p - path);
    *limit += len;
    if ((copy = malloc(len)) != NULL)
    {
        if (g_Ctoc(path, copy, len))
        {
            free(copy);
            return (GLOB_ABORTED);
        }
        pathv[pglob->gl_offs + pglob->gl_pathc++] = copy;
    }
    pathv[pglob->gl_offs + pglob->gl_pathc] = NULL;

    if ((pglob->gl_flags & GLOB_LIMIT) && (newsize + *limit) >= _POSIX_ARG_MAX)
    {
        __set_errno(0);
        return (GLOB_NOSPACE);
    }

    return (copy == NULL ? GLOB_NOSPACE : 0);
}

static int
glob1(char *pattern, glob_t *pglob, size_t *limit)
{
    char pathbuf[MAXPATHLEN + 1] = {0};

    /* A null pathname is invalid -- POSIX 1003.1 sect. 2.4. */
    if (*pattern == EOS)
        return (0);
    /*
	 * we save one character so that we can use ptr >= limit,
	 * in the general case when we are appending non nul chars only.
	 */
    return (glob2(pathbuf, pathbuf, pathbuf + sizeof(pathbuf) - 1, pattern,
                  pglob, limit));
}

static int
glob2(char *pathbuf, char *pathend, char *pathlim, char *pattern, glob_t *pglob, size_t *limit)
{
    struct STAT sb;
    char *p, *q;
    int anymeta;

    /*
	 * Loop over pattern segments until end of pattern or until
	 * segment with meta character found.
	 */
    for (anymeta = 0;;)
    {
        if (*pattern == EOS)
        { /* End of pattern? */
            *pathend = EOS;
            if (g_lstat(pathbuf, &sb, pglob))
                return (0);

            if (((pglob->gl_flags & GLOB_MARK) &&
                 pathend[-1] != SEP) &&
                (S_ISDIR(sb.st_mode) ||
                 (S_ISLNK(sb.st_mode) &&
                  (g_stat(pathbuf, &sb, pglob) == 0) &&
                  S_ISDIR(sb.st_mode))))
            {
                if (pathend >= pathlim)
                    return (GLOB_ABORTED);
                *pathend++ = SEP;
                *pathend = EOS;
            }
            ++pglob->gl_matchc;
            return (globextend(pathbuf, pglob, limit));
        }

        /* Find end of next segment, copy tentatively to pathend. */
        q = pathend;
        p = pattern;
        while (*p != EOS && *p != SEP)
        {
            if (ismeta(*p))
                anymeta = 1;
            if (q >= pathlim)
                return GLOB_ABORTED;
            *q++ = *p++;
        }

        if (!anymeta)
        { /* No expansion, do next segment. */
            pathend = q;
            pattern = p;
            while (*pattern == SEP)
            {
                if (pathend >= pathlim)
                    return GLOB_ABORTED;
                *pathend++ = *pattern++;
            }
        }
        else /* Need expansion, recurse. */
            return (glob3(pathbuf, pathend, pathlim, pattern, p,
                          pglob, limit));
    }
    /* NOTREACHED */
}

static int
glob3(char *pathbuf, char *pathend, char *pathlim, char *pattern, char *restpattern, glob_t *pglob, size_t *limit)
{
    struct dirent *dp;
    DIR *dirp;
    int error;
    char buf[MAXPATHLEN] = {0};

    /*
	 * The readdirfunc declaration can't be prototyped, because it is
	 * assigned, below, to two functions which are prototyped in glob.h
	 * and dirent.h as taking pointers to differently typed opaque
	 * structures.
	 */
    struct dirent *(*readdirfunc)__P((void *));

    *pathend = EOS;
    __set_errno(0);

    if ((dirp = g_opendir(pathbuf, pglob)) == NULL)
    {
        if (pglob->gl_errfunc)
        {
            if (g_Ctoc(pathbuf, buf, sizeof(buf)))
                return (GLOB_ABORTED);
            if (pglob->gl_errfunc(buf, __get_errno()) ||
                pglob->gl_flags & GLOB_ERR)
                return (GLOB_ABORTED);
        }
        /*
		 * Posix/XOpen: glob should return when it encounters a
		 * directory that it cannot open or read
		 * XXX: Should we ignore ENOTDIR and ENOENT though?
		 * I think that Posix had in mind EPERM...
		 */
        if (pglob->gl_flags & GLOB_ERR)
            return (GLOB_ABORTED);

        return (0);
    }

    error = 0;

    /* Search directory for matching names. */
    if (pglob->gl_flags & GLOB_ALTDIRFUNC)
        readdirfunc = pglob->gl_readdir;
    else
        readdirfunc = (struct dirent * (*)__P((void *))) readdir;
    while ((dp = (*readdirfunc)(dirp)) != NULL)
    {
        u_char *sc;
        char *dc;

        /* Initial DOT must be matched literally. */
        if (dp->d_name[0] == DOT && *pattern != DOT)
            continue;
        /*
		 * The resulting string contains EOS, so we can
		 * use the pathlim character, if it is the nul
		 */
        for (sc = (u_char *)dp->d_name, dc = pathend;
             dc <= pathlim && (*dc++ = *sc++) != EOS;)
            continue;

        /*
		 * Have we filled the buffer without seeing EOS?
		 */
        if (dc > pathlim && *pathlim != EOS)
        {
            /*
			 * Abort when requested by caller, otherwise
			 * reset pathend back to last SEP and continue
			 * with next dir entry.
			 */
            if (pglob->gl_flags & GLOB_ERR)
            {
                error = GLOB_ABORTED;
                break;
            }
            else
            {
                *pathend = EOS;
                continue;
            }
        }

        if (!match(pathend, pattern, restpattern))
        {
            *pathend = EOS;
            continue;
        }
        error = glob2(pathbuf, --dc, pathlim, restpattern, pglob, limit);
        if (error)
            break;
    }

    if (pglob->gl_flags & GLOB_ALTDIRFUNC)
        (*pglob->gl_closedir)(dirp);
    else
        closedir(dirp);

    /*
	 * Again Posix X/Open issue with regards to error handling.
	 */
    if ((error || __get_errno()) && (pglob->gl_flags & GLOB_ERR))
        return (GLOB_ABORTED);

    return (error);
}

static int
globexp2(const char *ptr, const char *pattern, glob_t *pglob, int *rv)
{
    int i;
    char *lm, *ls;
    const char *pe, *pm, *pl;
    char patbuf[MAXPATHLEN + 1] = {0};

    /* copy part up to the brace */
    for (lm = patbuf, pm = pattern; pm != ptr; *lm++ = *pm++)
        continue;
    ls = lm;

    /* Find the balanced brace */
    for (i = 0, pe = ++ptr; *pe; pe++)
        if (*pe == LBRACKET)
        {
            /* Ignore everything between [] */
            for (pm = pe++; *pe != RBRACKET && *pe != EOS; pe++)
                continue;
            if (*pe == EOS)
            {
                /* 
				 * We could not find a matching RBRACKET.
				 * Ignore and just look for RBRACE
				 */
                pe = pm;
            }
        }
        else if (*pe == LBRACE)
            i++;
        else if (*pe == RBRACE)
        {
            if (i == 0)
                break;
            i--;
        }

    /* Non matching braces; just glob the pattern */
    if (i != 0 || *pe == EOS)
    {
        /*
		 * we use `pattern', not `patbuf' here so that that
		 * unbalanced braces are passed to the match
		 */
        *rv = glob0(pattern, pglob);
        return 0;
    }

    for (i = 0, pl = pm = ptr; pm <= pe; pm++)
    {
        switch (*pm)
        {
        case LBRACKET:
            /* Ignore everything between [] */
            for (pl = pm++; *pm != RBRACKET && *pm != EOS; pm++)
                continue;
            if (*pm == EOS)
            {
                /* 
				 * We could not find a matching RBRACKET.
				 * Ignore and just look for RBRACE
				 */
                pm = pl;
            }
            break;

        case LBRACE:
            i++;
            break;

        case RBRACE:
            if (i)
            {
                i--;
                break;
            }
            /* FALLTHROUGH */
        case COMMA:
            if (i && *pm == COMMA)
                break;
            else
            {
                /* Append the current string */
                for (lm = ls; (pl < pm); *lm++ = *pl++)
                    continue;
                /* 
				 * Append the rest of the pattern after the
				 * closing brace
				 */
                for (pl = pe + 1; (*lm++ = *pl++) != EOS;)
                    continue;

                /* Expand the current pattern */

                *rv = globexp1(patbuf, pglob);

                /* move after the comma, to the next string */
                pl = pm + 1;
            }
            break;

        default:
            break;
        }
    }
    *rv = 0;
    return 0;
}

static const char *
globtilde(const char *pattern, char *patbuf, size_t patsize, glob_t *pglob)
{
    struct passwd *pwd;
    const char *h;
    const char *p;
    char *b;
    char *d;
    char *pend = &patbuf[patsize / sizeof(char)];

    pend--;

    if (*pattern != TILDE || !(pglob->gl_flags & GLOB_TILDE))
        return pattern;

    /* Copy up to the end of the string or / */
    for (p = pattern + 1, d = (char *)(void *)patbuf;
         d < (char *)(void *)pend && *p && *p != SLASH;
         *d++ = *p++)
        continue;

    if (d == (char *)(void *)pend)
        return NULL;

    *d = EOS;
    d = (char *)(void *)patbuf;

    if (*d == EOS)
    {
        /* 
		 * handle a plain ~ or ~/ by expanding $HOME 
		 * first and then trying the password file
		 */
        if ((h = getenv("HOME")) == NULL)
        {
            if ((pwd = getpwuid(getuid())) == NULL)
                return pattern;
            else
                h = pwd->pw_dir;
        }
    }
    else
    {
        /*
		 * Expand a ~user
		 */
        if ((pwd = getpwnam(d)) == NULL)
            return pattern;
        else
            h = pwd->pw_dir;
    }

    /* Copy the home directory */
    for (b = patbuf; b < pend && *h; *b++ = *h++)
        continue;

    if (b == pend)
        return NULL;

    /* Append the rest of the pattern */
    while (b < pend && (*b++ = *p++) != EOS)
        continue;

    if (b == pend)
        return NULL;

    return patbuf;
}

static int
glob0(const char *pattern, glob_t *pglob)
{
    const char *qpatnext;
    int c, error, oldpathc;
    char *bufnext, patbuf[MAXPATHLEN + 1] = {0};
    size_t limit = 0;

    if ((qpatnext = globtilde(pattern, patbuf, sizeof(patbuf),
                              pglob)) == NULL)
        return GLOB_ABEND;
    oldpathc = pglob->gl_pathc;
    bufnext = patbuf;

    /* We don't need to check for buffer overflow any more. */
    while ((c = *qpatnext++) != EOS)
    {
        switch (c)
        {
        case LBRACKET:
            c = *qpatnext;
            if (c == NOT)
                ++qpatnext;
            if (*qpatnext == EOS ||
                g_strchr(qpatnext + 1, RBRACKET) == NULL)
            {
                *bufnext++ = LBRACKET;
                if (c == NOT)
                    --qpatnext;
                break;
            }
            *bufnext++ = M_SET;
            if (c == NOT)
                *bufnext++ = M_NOT;
            c = *qpatnext++;
            do
            {
                *bufnext++ = CHAR(c);
                if (*qpatnext == RANGE &&
                    (c = qpatnext[1]) != RBRACKET)
                {
                    *bufnext++ = M_RNG;
                    *bufnext++ = CHAR(c);
                    qpatnext += 2;
                }
            } while ((c = *qpatnext++) != RBRACKET);
            pglob->gl_flags |= GLOB_MAGCHAR;
            *bufnext++ = M_END;
            break;
        case QUESTION:
            pglob->gl_flags |= GLOB_MAGCHAR;
            *bufnext++ = M_ONE;
            break;
        case STAR:
            pglob->gl_flags |= GLOB_MAGCHAR;
            /* collapse adjacent stars to one, 
			 * to avoid exponential behavior
			 */
            if (bufnext == patbuf || bufnext[-1] != M_ALL)
                *bufnext++ = M_ALL;
            break;
        default:
            *bufnext++ = CHAR(c);
            break;
        }
    }
    *bufnext = EOS;

    if ((error = glob1(patbuf, pglob, &limit)) != 0)
        return (error);

    if (pglob->gl_pathc == oldpathc)
    {
        /*
		 * If there was no match we are going to append the pattern 
		 * if GLOB_NOCHECK was specified or if GLOB_NOMAGIC was
		 * specified and the pattern did not contain any magic
		 * characters GLOB_NOMAGIC is there just for compatibility
		 * with csh.
		 */
        if ((pglob->gl_flags & GLOB_NOCHECK) ||
            ((pglob->gl_flags & (GLOB_NOMAGIC | GLOB_MAGCHAR)) == GLOB_NOMAGIC))
        {
            return globextend(pattern, pglob, &limit);
        }
        else
        {
            return (GLOB_NOMATCH);
        }
    }
    else if (!(pglob->gl_flags & GLOB_NOSORT))
    {
        qsort(pglob->gl_pathv + pglob->gl_offs + oldpathc,
              (size_t)pglob->gl_pathc - oldpathc, sizeof(char *),
              compare);
    }

    return (0);
}

static int
globexp1(const char *pattern, glob_t *pglob)
{
    const char *ptr = pattern;
    int rv;

    /* Protect a single {}, for find(1), like csh */
    if (pattern[0] == LBRACE && pattern[1] == RBRACE && pattern[2] == EOS)
        return glob0(pattern, pglob);

    while ((ptr = (const char *)g_strchr(ptr, LBRACE)) != NULL)
        if (!globexp2(ptr, pattern, pglob, &rv))
            return rv;

    return glob0(pattern, pglob);
}

static int
compare(const void *p, const void *q)
{
    return (strcoll(*(const char *const *)p, *(const char *const *)q));
}

/****************************************************************************/

int
glob(const char *pattern, int flags, int (*errfunc)(const char *epath, int eerrno), glob_t *pglob)
{
    const u_char *patnext;
    int c;
    char *bufnext, *bufend, patbuf[MAXPATHLEN + 1] = {0};

    ENTER();

    SHOWSTRING(pattern);
    SHOWVALUE(flags);
    SHOWPOINTER(pglob);

    patnext = (const u_char *)pattern;
    if (!(flags & GLOB_APPEND))
    {
        pglob->gl_pathc = 0;
        pglob->gl_pathv = NULL;
        if (!(flags & GLOB_DOOFFS))
            pglob->gl_offs = 0;
    }
    pglob->gl_flags = flags & ~GLOB_MAGCHAR;
    pglob->gl_errfunc = errfunc;
    pglob->gl_matchc = 0;

    bufnext = patbuf;
    bufend = bufnext + MAXPATHLEN;
    if (flags & GLOB_NOESCAPE)
    {
        while (bufnext < bufend && (c = *patnext++) != EOS)
            *bufnext++ = c;
    }
    else
    {
        /* Protect the quoted characters. */
        while (bufnext < bufend && (c = *patnext++) != EOS)
            if (c == QUOTE)
            {
                if ((c = *patnext++) == EOS)
                {
                    c = QUOTE;
                    --patnext;
                }
                *bufnext++ = c | M_PROTECT;
            }
            else
                *bufnext++ = c;
    }
    *bufnext = EOS;

    if (flags & GLOB_BRACE)
        return globexp1(patbuf, pglob);
    else
        return glob0(patbuf, pglob);
}