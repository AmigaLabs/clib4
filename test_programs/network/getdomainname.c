#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

static void
usage(void) {
    fprintf(stderr, "usage: %s [-h] [domain_name]\n", getexecname());
    fprintf(stderr, "  -h          show this help message\n");
    fprintf(stderr, "  domain_name set domain name (if not specified, prints current domain name)\n");

    exit(1);
}

int
main(int argc, char *argv[]) {
    int ch;
    char domainname[MAXHOSTNAMELEN];

    while ((ch = getopt(argc, argv, "h")) != -1) {
        switch (ch) {
            case 'h':
                usage();
                break;
            case '?':
                /* fall through */
            default:
                usage();
        }
    }
    argc -= optind;
    argv += optind;

    if (argc > 1)
        usage();

    if (*argv) {
        if (setdomainname(*argv, (int) strlen(*argv)))
            perror("setdomainname");
    } else {
        if (getdomainname(domainname, (int) sizeof(domainname)))
            perror("getdomainname");
        else
            printf("%s\n", domainname);
    }
    exit(0);
}