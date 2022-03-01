#include <langinfo.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[]) {
    setlocale(LC_CTYPE, "");
    setlocale(LC_NUMERIC, "");

    printf("%s\n", nl_langinfo(CODESET));
    printf("%s\n", nl_langinfo(RADIXCHAR));

    exit(EXIT_SUCCESS);
}