/*
 * $Id: unistd_getopt.c,v 1.7 2006-11-13 09:51:53 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int opterr = 1;
int optind = 1;
int optopt;
char *optarg;

int
getopt(int argc, char *const argv[], const char *opts) {
    static int sp = 1;
    int result = EOF;
    char *cp;
    int c;

    if (argv == NULL || opts == NULL) {
        errno = EFAULT;
        goto out;
    }

    if (sp == 1) {
        if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0') {
            goto out;
        } else if (strcmp(argv[optind], "--") == SAME) {
            optind++;

            goto out;
        }
    }

    optopt = c = argv[optind][sp];

    if (c == ':' || (cp = strchr(opts, c)) == NULL) {
        if (opterr != 0)
            fprintf(stderr, "%s%s%c\n", argv[0], ": illegal option -- ", c);

        if (argv[optind][++sp] == '\0') {
            optind++;

            sp = 1;
        }

        result = '?';
        goto out;
    }

    if (*++cp == ':') {
        if (argv[optind][sp + 1] != '\0') {
            optarg = &argv[optind++][sp + 1];
        } else if (++optind >= argc) {
            if (opterr != 0)
                fprintf(stderr, "%s%s%c\n", argv[0], ": option requires an argument -- ", c);

            sp = 1;

            result = '?';
            goto out;
        } else {
            optarg = argv[optind++];
        }

        sp = 1;
    } else {
        if (argv[optind][++sp] == '\0') {
            sp = 1;

            optind++;
        }

        optarg = NULL;
    }

    result = c;

out:

    return (result);
}
