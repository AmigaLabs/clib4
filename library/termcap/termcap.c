/*
 * $Id: termcap_tparam.c,v 1.1 2024-07-12 18:26:47 clib4devs Exp $
*/

#ifndef  _TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif

#include <term.h>

/* BUFSIZE is the initial size allocated for the buffer
   for reading the termcap file.
   It is not a limit.
   Make it large normally for speed.
*/

#ifndef BUFSIZE
#define BUFSIZE 8192
#endif

#ifndef TERMCAP_FILE
#define TERMCAP_FILE "ENV:termcap.os4"
#endif

/* Finding the termcap entry in the termcap data base.  */

struct termcap_buffer {
    char *beg;
    int size;
    char *ptr;
    int ateof;
    int full;
};

/* Forward declarations of static functions.  */
static int scan_file(char *str, int fd, register struct termcap_buffer *bufp);
static char *gobble_line(int fd, register struct termcap_buffer *bufp, char *append_end);
static int compare_contin(register char *str1, register char *str2);
static char *tgetst1(char *ptr, char **area);
static int name_match(char *line, char *name);

/* Search entry BP for capability CAP.
   Return a pointer to the capability (in BP) if found,
   0 if not found.  */

static char *
find_capability(const char *bp, const char *name) {
    for (; *bp; bp++)
        if (bp[0] == ':'
            && bp[1] == name[0]
            && bp[2] == name[1])
            return (char *) &bp[4];
    return NULL;
}

int
tgetnum(const char *name) {
    struct _clib4 *__clib4 = __CLIB4;

    register char *ptr = find_capability(__clib4->term_entry, name);
    if (!ptr || ptr[-1] != '#')
        return -1;
    return atoi(ptr);
}

int
tgetflag(const char *name) {
    struct _clib4 *__clib4 = __CLIB4;

    register char *ptr = find_capability(__clib4->term_entry, name);
    return ptr && ptr[-1] == ':';
}

/* Look up a string-valued capability CAP.
   If AREA is non-null, it points to a pointer to a block in which
   to store the string.  That pointer is advanced over the space used.
   If AREA is null, space is allocated with `malloc'.  */

char *
tgetstr(const char *name, char **area) {
    struct _clib4 *__clib4 = __CLIB4;

    register char *ptr = find_capability(__clib4->term_entry, name);
    if (!ptr || (ptr[-1] != '=' && ptr[-1] != '~'))
        return NULL;
    return tgetst1(ptr, area);
}

/* Table, indexed by a character in range 0100 to 0140 with 0100 subtracted,
   gives meaning of character following \, or a space if no special meaning.
   Eight characters per line within the string.  */

static char esctab[]
        = " \007\010  \033\014 \
      \012 \
  \015 \011 \013 \
        ";

/* PTR points to a string value inside a termcap entry.
   Copy that value, processing \ and ^ abbreviations,
   into the block that *AREA points to,
   or to newly allocated storage if AREA is NULL.
   Return the address to which we copied the value,
   or NULL if PTR is NULL.  */

static char *
tgetst1(char *ptr, char **area) {
    register char *p, *r;
    register int c;
    register int size;
    char *ret;
    register int c1;

    if (!ptr)
        return NULL;

    /* `ret' gets address of where to store the string.  */
    if (!area) {
        /* Compute size of block needed (may overestimate).  */
        p = ptr;
        while ((c = *p++) && c != ':' && c != '\n');
        ret = (char *) malloc(p - ptr + 1);
        if (ret == NULL) {
            __set_errno(ENOMEM);
            return NULL;
        }
    } else
        ret = *area;

    /* Copy the string value, stopping at null or colon.
       Also process ^ and \ abbreviations.  */
    p = ptr;
    r = ret;
    while ((c = *p++) && c != ':' && c != '\n') {
        if (c == '^') {
            c = *p++;
            if (c == '?')
                c = 0177;
            else
                c &= 037;
        } else if (c == '\\') {
            c = *p++;
            if (c >= '0' && c <= '7') {
                c -= '0';
                size = 0;

                while (++size < 3 && (c1 = *p) >= '0' && c1 <= '7') {
                    c *= 8;
                    c += c1 - '0';
                    p++;
                }
            } else if (c >= 0100 && c < 0200) {
                c1 = esctab[(c & ~040) - 0100];
                if (c1 != ' ')
                    c = c1;
            }
        }
        *r++ = c;
    }
    *r = '\0';
    /* Update *AREA.  */
    if (area)
        *area = r + 1;
    return ret;
}

/* Actual baud rate if positive;
   - baud rate / 100 if negative.  */

static int speeds[] = {
    0, 50, 75, 110, 135, 150, -2, -3, -6, -12, -18, -24, -48, -96, -192, -288, -384, -576, -1152
};

void
tputs(const char *str, int nlines, register int (*outfun)()) {
    register int padcount = 0;
    register int speed;
    size_t n_speeds = sizeof(speeds) / sizeof(speeds[0]);
    struct _clib4 *__clib4 = __CLIB4;

    __check_abort_f(__clib4);

    if (__clib4->__ospeed == 0)
        speed = __clib4->__tputs_baud_rate;
    else if (__clib4->__ospeed >= n_speeds)
        speed = speeds[n_speeds - 1];
    else
        speed = speeds[__clib4->__ospeed];

    if (!str)
        return;

    while (*str >= '0' && *str <= '9') {
        padcount += *str++ - '0';
        padcount *= 10;
    }
    if (*str == '.') {
        str++;
        padcount += *str++ - '0';
    }
    if (*str == '*') {
        str++;
        padcount *= nlines;
    }
    while (*str)
        (*outfun)(*str++);

    /* PADCOUNT is now in units of tenths of msec.
       SPEED is measured in characters per 10 seconds
       or in characters per .1 seconds (if negative).
       We use the smaller units for larger speeds to avoid overflow.  */
    padcount *= speed;
    padcount += 500;
    padcount /= 1000;
    if (speed < 0)
        padcount = -padcount;
    else {
        padcount += 50;
        padcount /= 100;
    }

    while (padcount-- > 0)
        (*outfun)(__clib4->__PC);
}

/* Find the termcap entry data for terminal type NAME
   and store it in the block that BP points to.
   Record its address for future use.

   If BP is null, space is dynamically allocated.

   Return -1 if there is some difficulty accessing the data base
   of terminal types,
   0 if the data base is accessible but the type NAME is not defined
   in it, and some other value otherwise.  */

int
tgetent(char *bp, const char *name) {
    const char *termcap_name;
    register int fd;
    struct termcap_buffer buf;
    register char *bp1;
    char *tc_search_point;
    char *term;
    int malloc_size = 0;
    register int c;
    char *tcenv = NULL;        /* TERMCAP value, if it contains :tc=.  */
    char *indirect = NULL;    /* Terminal type in :tc= in TERMCAP value.  */
    struct _clib4 *__clib4 = __CLIB4;

    __check_abort_f(__clib4);

    /* For compatibility with programs like `less' that want to
       put data in the termcap buffer themselves as a fallback.  */
    if (bp)
        __clib4->term_entry = bp;
    termcap_name = getenv("TERMCAP");

    if (termcap_name && *termcap_name == '\0')
        termcap_name = NULL;

    if (termcap_name && !strcmp(name, getenv("TERM"))) {
        indirect = tgetst1(find_capability(termcap_name, "tc"), (char **) 0);
        if (!indirect) {
            if (!bp)
                bp = (char *) termcap_name;
            else
                strcpy(bp, termcap_name);
            goto ret;
        } else {            /* It has tc=.  Need to read /etc/termcap.  */
            tcenv = (char *) termcap_name;
            termcap_name = NULL;
        }
    }

    if (!termcap_name)
        termcap_name = TERMCAP_FILE;

    /* Here we know we must search a file and termcap_name has its name.  */
    fd = open(termcap_name, O_RDONLY, 0);
    if (fd < 0) {
        return -1;
    }
    buf.size = BUFSIZE;
    /* Add 1 to size to ensure room for terminating null.  */
    buf.beg = (char *) malloc(buf.size + 1);
    if (buf.beg == NULL) {
        __set_errno(ENOMEM);
        return -1;
    }

    term = indirect ? indirect : (char *) name;

    if (!bp) {
        malloc_size = indirect ? strlen(tcenv) + 1 : buf.size;
        bp = (char *) malloc(malloc_size);
        if (bp == NULL) {
            __set_errno(ENOMEM);
            return -1;
        }
    }
    tc_search_point = bp1 = bp;

    if (indirect) {
        /* Copy the data from the environment variable.  */
        strcpy(bp, tcenv);
        bp1 += strlen(tcenv);
    }

    while (term) {
        /* Scan the file, reading it via buf, till find start of main entry.  */
        if (scan_file(term, fd, &buf) == 0) {
            close(fd);
            free(buf.beg);
            if (malloc_size)
                free(bp);
            return -1;
        }

        /* Free old `term' if appropriate.  */
        if (term != name)
            free(term);

        /* If BP is malloc'd by us, make sure it is big enough.  */
        if (malloc_size) {
            int offset1 = bp1 - bp, offset2 = tc_search_point - bp;
            malloc_size = offset1 + buf.size;
            bp = (char *) realloc(bp, malloc_size);
            termcap_name = (char *) realloc(bp, malloc_size);
            if (bp == NULL || termcap_name == NULL) {
                __set_errno(ENOMEM);
                return -1;
            }
            bp1 = (char *) termcap_name + offset1;
            tc_search_point = (char *) termcap_name + offset2;
        }

        /* Copy the line of the entry from buf into bp.  */
        termcap_name = buf.ptr;
        while ((*bp1++ = c = *termcap_name++) && c != '\n')
            /* Drop out any \ newline sequence.  */
            if (c == '\\' && *termcap_name == '\n') {
                bp1--;
                termcap_name++;
            }
        *bp1 = '\0';

        /* Does this entry refer to another terminal type's entry?
       If something is found, copy it into heap and null-terminate it.  */
        tc_search_point = find_capability(tc_search_point, "tc");
        term = tgetst1(tc_search_point, (char **) 0);
    }

    close(fd);
    free(buf.beg);

    if (malloc_size) {
        bp = (char *) realloc(bp, bp1 - bp + 1);
        if (bp == NULL) {
            __set_errno(ENOMEM);
            return -1;
        }
    }

ret:
    __clib4->term_entry = bp;
    return 1;
}

/* Given file open on FD and buffer BUFP,
   scan the file from the beginning until a line is found
   that starts the entry for terminal type STR.
   Return 1 if successful, with that line in BUFP,
   or 0 if no entry is found in the file.  */

static int
scan_file(char *str, int fd, register struct termcap_buffer *bufp) {
    register char *end;

    bufp->ptr = bufp->beg;
    bufp->full = 0;
    bufp->ateof = 0;
    *bufp->ptr = '\0';

    lseek(fd, 0L, 0);

    while (!bufp->ateof) {
        /* Read a line into the buffer.  */
        end = NULL;
        do {
            /* if it is continued, append another line to it,
               until a non-continued line ends.  */
            end = gobble_line(fd, bufp, end);
        } while (!bufp->ateof && end[-2] == '\\');

        if (*bufp->ptr != '#'
            && name_match(bufp->ptr, str))
            return 1;

        /* Discard the line just processed.  */
        bufp->ptr = end;
    }
    return 0;
}

/* Return nonzero if NAME is one of the names specified
   by termcap entry LINE.  */

static int
name_match(char *line, char *name) {
    register char *tem;

    if (!compare_contin(line, name))
        return 1;
    /* This line starts an entry.  Is it the right one?  */
    for (tem = line; *tem && *tem != '\n' && *tem != ':'; tem++)
        if (*tem == '|' && !compare_contin(tem + 1, name))
            return 1;

    return 0;
}

static int
compare_contin(register char *str1, register char *str2) {
    register int c1, c2;
    while (1) {
        c1 = *str1++;
        c2 = *str2++;
        while (c1 == '\\' && *str1 == '\n') {
            str1++;
            while ((c1 = *str1++) == ' ' || c1 == '\t');
        }
        if (c2 == '\0') {
            /* End of type being looked up.  */
            if (c1 == '|' || c1 == ':')
                /* If end of name in data base, we win.  */
                return 0;
            else
                return 1;
        } else if (c1 != c2)
            return 1;
    }
}

/* Make sure that the buffer <- BUFP contains a full line
   of the file open on FD, starting at the place BUFP->ptr
   points to.  Can read more of the file, discard stuff before
   BUFP->ptr, or make the buffer bigger.

   Return the pointer to after the newline ending the line,
   or to the end of the file, if there is no newline to end it.

   Can also merge on continuation lines.  If APPEND_END is
   non-null, it points past the newline of a line that is
   continued; we add another line onto it and regard the whole
   thing as one line.  The caller decides when a line is continued.  */

static char *
gobble_line(int fd, register struct termcap_buffer *bufp, char *append_end) {
    register char *end;
    register int nread;
    register char *buf = bufp->beg;
    register char *tem;

    if (!append_end)
        append_end = bufp->ptr;

    while (1) {
        end = append_end;
        while (*end && *end != '\n') end++;
        if (*end)
            break;
        if (bufp->ateof)
            return buf + bufp->full;
        if (bufp->ptr == buf) {
            if (bufp->full == bufp->size) {
                bufp->size *= 2;
                /* Add 1 to size to ensure room for terminating null.  */
                tem = (char *) realloc(buf, bufp->size + 1);
                if (tem == NULL) {
                    __set_errno(ENOMEM);
                    return NULL;
                }
                bufp->ptr = (bufp->ptr - buf) + tem;
                append_end = (append_end - buf) + tem;
                bufp->beg = buf = tem;
            }
        } else {
            append_end -= bufp->ptr - buf;
            bcopy(bufp->ptr, buf, bufp->full -= bufp->ptr - buf);
            bufp->ptr = buf;
        }
        if (!(nread = read(fd, buf + bufp->full, bufp->size - bufp->full)))
            bufp->ateof = 1;
        bufp->full += nread;
        buf[bufp->full] = '\0';
    }
    return end + 1;
}


