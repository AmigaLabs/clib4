#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>

static void
usage(void) {
    fprintf(stderr, "usage: %s domain_name\n", __program_name);

    exit(1);
}

int
main(int argc, char *argv[]) {
    int ch;
    char domainname[MAXHOSTNAMELEN];

    while ((ch = getopt(argc, argv, "")) != -1) {
        switch (ch) {
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