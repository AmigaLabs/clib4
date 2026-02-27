#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

char *
make_message(const char *fmt, ...) {
    int n;
    int size = 1000;     /* Guess we need no more than 100 bytes */
    char *p, *np;
    va_list ap;

    if ((p = malloc(size)) == NULL)
        return NULL;

    while (1) {
        /* Try to print in the allocated space */
        va_start(ap, fmt);
        n = vsnprintf(p, size, fmt, ap);
        va_end(ap);

        /* Check error code */
        if (n < 0)
            return NULL;

        /* If that worked, return the string */
        if (n < size)
            return p;

        /* Else try again with more space */
        size = n + 1;       /* Precisely what is needed */

        if ((np = realloc(p, size)) == NULL) {
            free(p);
            return NULL;
        } else {
            p = np;
        }
    }
}

int main() {
    char *p = make_message("%s %d\n", "This is a message", 3);

    printf("'%s'\n", p);

    return 0;
}