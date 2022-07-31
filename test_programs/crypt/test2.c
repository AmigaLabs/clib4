#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>

int
main(void) {
    /* Hashed form of "AMIGA RULEZ". */
    const char *const pass = "$1$ggbtWVOE$sqZlTmnRKyRJV/2r.TVVP/";

    char *result;
    int ok;
    char *passwd = getpass("Password:");

    /* Read in the user's password and encrypt it,
       passing the expected password in as the salt. */
    result = crypt(passwd, pass);

    /* Test the result. */
    ok = strcmp(result, pass) == 0;

    puts(ok ? "Access granted." : "Access denied.");
    return ok ? 0 : 1;
}