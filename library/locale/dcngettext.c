/*
 * $Id: locale_dcngettext.c,v 1.0 2023-06-08 14:51:15 clib4devs Exp $
*/

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <atomic.h>
#include <endian.h>
#include <libintl.h>
#include <sys/mman.h>
#include <sys/syslimits.h>

#include "dcngettext.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const char *g_catname[] = {
    "LC_ALL",
    "LC_COLLATE",
    "LC_CTYPE",
    "LC_MONETARY",
    "LC_NUMERIC",
    "LC_TIME",
    "LC_MESSAGES",
    "LC_PAPER",
    "LC_NAME",
    "LC_ADDRESS",
    "LC_TELEPHONE",
    "LC_MEASUREMENT",
    "LC_IDENTIFICATION",
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static const char *evalexpr(struct eval_s *ev, const char *s, int d);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static char *gettextdomain(void) {
    return __CLIB4->gettext_domain;
}

static char *
gettextdir(const char *domainname, size_t *dirlen) {
    struct binding *p;
    struct _clib4 *__clib4 = __CLIB4;

    for (p = __clib4->bindings; p; p = p->next) {
        if (!strcmp(p->domainname, domainname) && p->active) {
            *dirlen = p->dirlen;
            return (char *)p->dirname;
        }
    }
    return 0;
}

/* MO file format is documented here:
 * https://www.gnu.org/software/gettext/manual/gettext.html#MO-Files
 */

static void *momap(const char *path, size_t *size) {
    uint32_t *map = MAP_FAILED;
    struct stat st;
    int fd;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        return map;
    }

    if (fstat(fd, &st) >= 0) {
        *size = st.st_size;
        map = mmap(NULL, *size, PROT_READ, MAP_SHARED, fd, 0);
        if (map != MAP_FAILED &&
            map[0] != MO_MAGIC && map[0] != __bswap_32(MO_MAGIC)) {
            munmap(map, *size);
            map = MAP_FAILED;
        }
    }

    close(fd);
    return map;
}

static inline uint32_t
moswap(uint32_t x, int sw) {
    return sw ? __bswap_32(x) : x;
}

static char *
molookup(char *p, size_t size, const char *s) {
    uint32_t * mo = (uint32_t * )
    p;
    int sw = mo[0] - MO_MAGIC;
    uint32_t n = moswap(mo[2], sw);
    uint32_t o = moswap(mo[3], sw);
    uint32_t t = moswap(mo[4], sw);
    uint32_t b = 0;

    if (n > size / 8 || o > size - 8 * n ||
        t > size - 8 * n || (o | t) % 4 != 0) {
        return NULL;
    }

    o /= 4;
    t /= 4;

    for (;;) {
        uint32_t ol = moswap(mo[o + 2 * (b + n / 2)], sw);
        uint32_t os = moswap(mo[o + 2 * (b + n / 2) + 1], sw);
        int sign;

        if (ol >= size || os >= size - ol || p[os + ol]) {
            return NULL;
        }

        sign = strcmp(s, p + os);
        if (sign == 0) {
            uint32_t tl = moswap(mo[t + 2 * (b + n / 2)], sw);
            uint32_t ts = moswap(mo[t + 2 * (b + n / 2) + 1], sw);

            if (tl >= size || ts >= size - tl || p[ts + tl]) {
                return NULL;
            }

            return p + ts;
        } else if (n == 1) {
            return NULL;
        } else if (sign < 0) {
            n /= 2;
        } else {
            b += n / 2;
            n -= n / 2;
        }
    }

    return NULL;
}

static const char *
skipspace(const char *s) {
    while (isspace(*s)) {
        s++;
    }

    return s;
}

/* Grammar:
 *
 * Start = Expr ';'
 * Expr  = Or | Or '?' Expr ':' Expr
 * Or    = And | Or '||' And
 * And   = Eq | And '&&' Eq
 * Eq    = Rel | Eq '==' Rel | Eq '!=' Rel
 * Rel   = Add | Rel '<=' Add | Rel '>=' Add | Rel '<' Add | Rel '>' Add
 * Add   = Mul | Add '+' Mul | Add '-' Mul
 * Mul   = Prim | Mul '*' Prim | Mul '/' Prim | Mul '%' Prim
 * Prim  = '(' Expr ')' | '!' Prim | decimal | 'n'
 *
 * Internals:
 *
 * Recursive descent expression evaluator with stack depth limit.
 * for binary operators an operator-precedence parser is used.
 * eval* functions store the result of the parsed subexpression
 * and return a pointer to the next non-space character.
 */

static const char *
evalprim(struct eval_s *ev, const char *s, int d) {
    char *e;

    if (--d < 0) {
        return "";
    }

    s = skipspace(s);
    if (isdigit(*s)) {
        ev->r = strtoul(s, &e, 10);
        if (e == s || ev->r == -1) {
            return "";
        }

        return skipspace(e);
    }

    if (*s == 'n') {
        ev->r = ev->n;
        return skipspace(s + 1);
    }

    if (*s == '(') {
        s = evalexpr(ev, s + 1, d);
        if (*s != ')') {
            return "";
        }

        return skipspace(s + 1);
    }

    if (*s == '!') {
        s = evalprim(ev, s + 1, d);
        ev->r = !ev->r;
        return s;
    }

    return "";
}

static BOOL
binop(struct eval_s *ev, int op, unsigned long left) {
    unsigned long a = left;
    unsigned long b = ev->r;

    switch (op) {
        case 0:
            ev->r = a || b;
            return TRUE;

        case 1:
            ev->r = a && b;
            return TRUE;

        case 2:
            ev->r = a == b;
            return TRUE;

        case 3:
            ev->r = a != b;
            return TRUE;

        case 4:
            ev->r = a >= b;
            return TRUE;

        case 5:
            ev->r = a <= b;
            return TRUE;

        case 6:
            ev->r = a > b;
            return TRUE;

        case 7:
            ev->r = a < b;
            return TRUE;

        case 8:
            ev->r = a + b;
            return TRUE;

        case 9:
            ev->r = a - b;
            return TRUE;

        case 10:
            ev->r = a * b;
            return TRUE;

        case 11:
            if (b) {
                ev->r = a % b;
                return TRUE;
            }

            return FALSE;

        case 12:
            if (b) {
                ev->r = a / b;
                return TRUE;
            }

            return FALSE;
    }

    return FALSE;
}

static const char *parseop(struct eval_s *ev, const char *s) {
    static const char opch[] = {
        '|', '&', '=', '!', '>', '<', '+', '-', '*', '%', '/'
    };

    static const char opch2[] = {
        '|', '&', '=', '=', '=', '='
    };

    int i;

    for (i = 0; i < sizeof(opch); i++) {
        if (*s == opch[i]) {
            /* note: >,< are accepted with or without = */

            if (i < sizeof(opch2) && *(s + 1) == opch2[i]) {
                ev->op = i;
                return s + 2;
            }

            if (i >= 4) {
                ev->op = i + 2;
                return s + 1;
            }

            break;
        }
    }

    ev->op = 13;
    return s;
}

static const char *
evalbinop(struct eval_s *ev, const char *s, int minprec, int d) {
    static const char prec[14] = {
        1,
        2,
        3,
        3,
        4,
        4,
        4,
        4,
        5,
        5,
        6,
        6,
        6,
        0
    };

    unsigned long left;
    int op;

    s = evalprim(ev, s, --d);
    s = parseop(ev, s);

    for (;;) {
        /* ev->r (left hand side value) and ev->op are now set,
         * get the right hand side or back out if op has low prec,
         * if op was missing then prec[op]==0
         */

        op = ev->op;
        if (prec[op] <= minprec) {
            return s;
        }

        left = ev->r;
        s = evalbinop(ev, s, prec[op], d);
        if (!binop(ev, op, left)) {
            return "";
        }
    }
}

static const char *
evalexpr(struct eval_s *ev, const char *s, int d) {
    unsigned long a;
    unsigned long b;

    s = evalbinop(ev, s, 0, --d);
    if (*s != '?') {
        return s;
    }

    a = ev->r;
    s = evalexpr(ev, s + 1, d);
    if (*s != ':') {
        return "";
    }

    b = ev->r;
    s = evalexpr(ev, s + 1, d);
    ev->r = a ? b : ev->r;

    return s;
}

static unsigned long
eval(const char *s, unsigned long n) {
    struct eval_s ev;

    memset(&ev, 0, sizeof(ev));

    ev.n = n;
    s = evalexpr(&ev, s, 100);

    return *s == ';' ? ev.r : -1;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: dcngettext
 *
 * Description:
 *   The dcngettext function searches the message catalogs identified by
 *   domainname and category for a string which is equal to msgid1.
 *
 *   The msgid1 parameter must contain the singular form of the string to
 *   be converted. It is also used as the key for the search in the catalog.
 *   The msgid2 parameter is the plural form. The parameter n is used to
 *   determine the plural form.
 *
 * Returned Value:
 *   If there is such a string available it is returned. Otherwise msgid1 is
 *   returned if n == 1, otherwise msgid2.
 *
 *   Please note that although the return value is char * the returned string
 *   must not be changed. This broken type results from the history of the
 *   function and does not reflect the way the function should be used.
 *
 ****************************************************************************/

char *gettext(const char *msgid) {
    return dgettext(NULL, msgid);
}

char *dgettext(const char *domainname, const char *msgid) {
    return dcgettext(domainname, msgid, LC_MESSAGES);
}

char *dcgettext(const char *domainname, const char *msgid, int category) {
    return dcngettext(domainname, msgid, NULL, 1, category);
}

char *ngettext(const char *msgid1, const char *msgid2, unsigned long int n) {
    return dngettext(NULL, msgid1, msgid2, n);
}

char *dngettext(const char *domainname, const char *msgid1, const char *msgid2, unsigned long int n) {
    return dcngettext(domainname, msgid1, msgid2, n, LC_MESSAGES);
}

char *dcngettext(const char *domainname, const char *msgid1, const char *msgid2, unsigned long int n, int category) {
    struct mofile_s *mofile;
    const char *lang;
    char path[PATH_MAX];
    char *notrans;
    char *trans;
    const char *dirname;
    struct binding *q;
    struct _clib4 *__clib4 = __CLIB4;

    notrans = (char *) (n == 1 ? msgid1 : msgid2);

    if (msgid1 == NULL) {
        return notrans;
    }

    if (category < 0 || category >= LC_MAX) {
        return notrans;
    }

    if (domainname == NULL) {
        domainname = textdomain(NULL);
    }

    lang = getenv("LANG");
    if (lang == NULL) {
        lang = getenv("LANGUAGE");
        if (lang == NULL) {
            lang = "C";
        }
    }

    for (q = __clib4->bindings; q; q = q->next)
        if (!strcmp(q->domainname, domainname) && q->active)
            break;
    if (q) {
        dirname = q->dirname;
        if (__clib4->__unix_path_semantics) {
            snprintf(path, PATH_MAX, "%s/%s/%s/%s.mo", dirname, lang, g_catname[category], domainname);
        }
        else {
            const int len = strlen(dirname);
            if (len > 0 && dirname[len-1] == ':' ) {
                snprintf(path, PATH_MAX, "%s%s/%s/%s.mo", dirname, lang, g_catname[category], domainname);
            }
            else
                snprintf(path, PATH_MAX, "%s/%s/%s/%s.mo", dirname, lang, g_catname[category], domainname);
        }
    }
    else {
        if (__clib4->__unix_path_semantics)
            snprintf(path, PATH_MAX, "./%s/%s/%s.mo", lang, g_catname[category], domainname);
        else
            snprintf(path, PATH_MAX, "CURRDIR:%s/%s/%s.mo", lang, g_catname[category], domainname);
    }

    ObtainSemaphore(__clib4->gettext_lock);

    for (mofile = __clib4->g_mofile; mofile; mofile = mofile->next) {
        if (strcmp(mofile->path, path) == 0) {
            break;
        }
    }

    if (mofile == NULL) {
        const char *r;

        mofile = __calloc_r(__clib4, 1, sizeof(*mofile));
        if (mofile == NULL) {
            ReleaseSemaphore(__clib4->gettext_lock);
            return notrans;
        }

        strlcpy(mofile->path, path, PATH_MAX);
        mofile->map = momap(path, &mofile->size);
        if (mofile->map == MAP_FAILED) {
            ReleaseSemaphore(__clib4->gettext_lock);
            __free_r(__clib4, mofile);
            return notrans;
        }

        /* Initialize the default plural rule */

        mofile->plural_rule = "n!=1;";
        mofile->nplurals = 2;

        /* Parse the plural rule from the header entry(empty string) */

        r = molookup(mofile->map, mofile->size, "");
        while (r != NULL && strncmp(r, "Plural-Forms:", 13)) {
            r = strchr(r, '\n');
            if (r != NULL) {
                r += 1;
            }
        }

        if (r != NULL) {
            r = skipspace(r + 13);
            if (strncmp(r, "nplurals=", 9) == 0) {
                mofile->nplurals = strtoul(r + 9, (char **) &r, 10);
            }

            r = strchr(r, ';');
            if (r != NULL) {
                r = skipspace(r + 1);
                if (strncmp(r, "plural=", 7) == 0) {
                    mofile->plural_rule = r + 7;
                }
            }
        }

        mofile->next = __clib4->g_mofile;
        __clib4->g_mofile = mofile;
    }

    ReleaseSemaphore(__clib4->gettext_lock); /* Leave look before search */

    trans = molookup(mofile->map, mofile->size, msgid1);
    if (trans == NULL) {
        return notrans;
    }

    /* Process the plural rule if request */

    if (msgid2 && mofile->nplurals) {
        unsigned long plural = eval(mofile->plural_rule, n);
        if (plural >= mofile->nplurals) {
            return notrans;
        }

        while (plural-- != 0) {
            size_t rem = mofile->size - (trans - (char *) mofile->map);
            size_t l = strnlen(trans, rem);

            if (l + 1 >= rem) {
                return notrans;
            }

            trans += l + 1;
        }
    }

    return trans;
}

/****************************************************************************
 * Name: textdomain
 *
 * Description:
 *   The textdomain function sets the default domain, which is used in all
 *   future gettext calls, to domainname. Please note that dgettext and
 *   dcgettext calls are not influenced if the domainname parameter of these
 *   functions is not the null pointer.
 *
 *   Before the first call to textdomain the default domain is messages. This
 *   is the name specified in the specification of the gettext API. This name
 *   is as good as any other name. No program should ever really use a domain
 *   with this name since this can only lead to problems.
 *
 *   If the domainname parameter is the empty string the default domain is
 *   reset to its initial value, the domain with the name messages. This
 *   possibility is questionable to use since the domain messages really
 *   never should be used.
 *
 * Returned Value:
 *   The function returns the value which is from now on taken as the default
 *   domain. If the system went out of memory the returned value is NULL and
 *   the global variable errno is set to ENOMEM. Despite the return value
 *   type being char * the return string must not be changed. It is allocated
 *   internally by the textdomain function.
 *
 *   If the domainname parameter is the null pointer no new default domain is
 *   set. Instead the currently selected default domain is returned.
 *
 ****************************************************************************/

char *textdomain(const char *domainname) {
    char *domain;
    size_t domainlen;

    domain = gettextdomain();
    if (domainname == NULL) {
        return *domain ? domain : "messages";
    }

    domainlen = strnlen(domainname, NAME_MAX);
    if (domainlen == NAME_MAX) {
        __set_errno(ENOMEM);
        return NULL;
    }

    memcpy(domain, domainname, domainlen + 1);
    return domain;
}

/****************************************************************************
 * Name: bindtextdomain
 *
 * Description:
 *   The bindtextdomain function can be used to specify the directory which
 *   contains the message catalogs for domain domainname for the different
 *   languages. To be correct, this is the directory where the hierarchy of
 *   directories is expected. Details are explained below.
 *
 *   For the programmer it is important to note that the translations which
 *   come with the program have to be placed in a directory hierarchy
 *   starting at, say, /foo/bar. Then the program should make a
 *   bindtextdomain call to bind the domain for the current program to this
 *   directory. So it is made sure the catalogs are found. A correctly
 *   running program does not depend on the user setting an environment
 *   variable.
 *
 *   The bindtextdomain function can be used several times and if the
 *   domainname argument is different the previously bound domains will not
 *   be overwritten.
 *
 *   If the program which wish to use bindtextdomain at some point of time
 *   use the chdir function to change the current working directory it is
 *   important that the dirname strings ought to be an absolute pathname.
 *   Otherwise the addressed directory might vary with the time.
 *
 * Returned Value:
 *   If the dirname parameter is the null pointer bindtextdomain returns the
 *   currently selected directory for the domain with the name domainname.
 *
 *   The bindtextdomain function returns a pointer to a string containing the
 *   name of the selected directory name. The string is allocated internally
 *   in the function and must not be changed by the user. If the system went
 *   out of core during the execution of bindtextdomain the return value is
 *   NULL and the global variable errno is set accordingly.
 *
 ****************************************************************************/

char *bindtextdomain(const char *domainname, const char *dirname) {
    struct _clib4 *__clib4 = __CLIB4;
    struct name_translation_info path_name_nti;
    struct binding *p, *q;

    if (domainname == NULL) {
        __set_errno(EINVAL);
        return NULL;
    }

    if (!dirname)
        return gettextdir(domainname, &(size_t) { 0 });

    size_t domlen = strnlen(domainname, NAME_MAX + 1);
    size_t dirlen = strnlen(dirname, PATH_MAX);
    if (domlen > NAME_MAX || dirlen >= PATH_MAX) {
        __set_errno(EINVAL);
        return NULL;
    }

    ObtainSemaphore(__clib4->gettext_lock);
    for (p = __clib4->bindings; p; p = p->next) {
        if (!strcmp(p->domainname, domainname) && !strcmp(p->dirname, dirname)) {
            break;
        }
    }

    if (!p) {
        p = __calloc_r(__clib4, 1, sizeof *p + domlen + dirlen + 2);
        if (!p) {
            ReleaseSemaphore(__clib4->gettext_lock);
            return NULL;
        }

        p->next = __clib4->bindings;
        p->dirlen = dirlen;
        p->domainname = p->buf;
        p->dirname = p->buf + domlen + 1;
        memcpy(p->domainname, domainname, domlen+1);
        memcpy(p->dirname, dirname, dirlen+1);
        __clib4->bindings = p;
        /* For some reason using atomic operations here is causing a DSI but
         * sice clib4 is reentrant should not be a problem using the assignment
         * WAS: a_cas_p(&__clib4->bindings, __clib4->bindings, p);
         */
    }

    a_store(&p->active, 1);

    for (q = __clib4->bindings; q; q = q->next) {
        if (!strcmp(q->domainname, domainname) && q != p)
            a_store(&q->active, 0);
    }

    ReleaseSemaphore(__clib4->gettext_lock);

    return (char *) p->dirname;
}

/****************************************************************************
 * Name: bind_textdomain_codeset
 *
 * Description:
 *   The bind_textdomain_codeset function can be used to specify the output
 *   character set for message catalogs for domain domainname. The codeset
 *   argument must be a valid codeset name which can be used for the
 *   iconv_open function, or a null pointer.
 *
 *   The bind_textdomain_codeset function can be used several times. If used
 *   multiple times with the same domainname argument, the later call
 *   overrides the settings made by the earlier one.
 *
 * Returned Value:
 *   If the codeset parameter is the null pointer, bind_textdomain_codeset
 *   returns the currently selected codeset for the domain with the name
 *   domainname. It returns NULL if no codeset has yet been selected.
 *
 *   The bind_textdomain_codeset function returns a pointer to a string
 *   containing the name of the selected codeset. The string is allocated
 *   internally in the function and must not be changed by the user. If the
 *   system went out of core during the execution of bind_textdomain_codeset,
 *   the return value is NULL and the global variable errno is set
 *   accordingly.
 *
 ****************************************************************************/

char *bind_textdomain_codeset(const char *domainname, const char *codeset) {
    if (codeset && strcasecmp(codeset, "UTF-8")) {
        __set_errno(EINVAL); /* Only support utf8 */
    }

    return NULL;
}

CLIB_DESTRUCTOR(dcngettext_exit) {
    struct _clib4 *__clib4 = __CLIB4;
    /* Free binddtextdomain bindings */
    if (__clib4->g_mofile) {
        struct mofile_s *mofile = __clib4->g_mofile;
        while (mofile) {
            struct mofile_s *next = mofile->next;
            if (mofile)
                free(mofile);
            mofile = next;
        }
    }
    struct binding *p = __clib4->bindings;
    while (p) {
        struct binding *q = p->next;
        free(p);
        p = q;
    }
}