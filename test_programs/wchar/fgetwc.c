#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <errno.h>
#include <locale.h>

int main(void) {
    setlocale(LC_ALL, "C-UTF-8");
    //setlocale(LC_ALL, "en_US.utf8");
    FILE *fp = fopen("fgetwc.dat", "w");
    if (!fp) {
        perror("Can't open file for writing");
        return EXIT_FAILURE;
    }
    printf("nchars=%ld\n", wcslen(L"кошка\n"));
    fputws(L"кошка\n", fp);
    fclose(fp);

    fp = fopen("fgetwc.dat", "r");
    if (!fp) {
        perror("Can't open file for reading");
        return EXIT_FAILURE;
    }
    wint_t wc;
    errno = 0;
    FILE *fp1 = fopen("fgetwc.out", "w");
    while ((wc = fgetwc(fp)) != WEOF)
        fputwc(wc, fp1);

    if (ferror(fp)) {
        if (errno == EILSEQ)
            puts("Character encoding error while reading.");
        else
            puts("I/O error when reading");
    } else if (feof(fp))
        puts("End of file reached successfully");

    fclose(fp);
    fclose(fp1);
}