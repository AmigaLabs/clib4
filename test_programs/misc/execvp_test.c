/*
 * $Id: execvp_test.c,v 1.2 2006-08-07 08:39:56 clib4devs Exp $
 */

#include <unistd.h>

int
main(int argc, char **argv) {
    if (argc > 1)
        execvp(argv[1], &argv[1]);

    return (0);
}
