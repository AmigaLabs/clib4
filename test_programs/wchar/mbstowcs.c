#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int len;
    char *pmbnull = NULL;
    char *pmb = (char *) malloc(MB_CUR_MAX);
    wchar_t *pwc = L"Hi";
    wchar_t *pwcs = (wchar_t *) malloc(sizeof(wchar_t));

    if (pwcs != NULL) {
        printf("Converting to multibyte string\n");
        len = wcstombs(pmb, pwc, MB_CUR_MAX);
        printf("Characters converted %d\n", len);
        printf("Hex value of first multibyte character: %#.4x\n", pmb);

        printf("Converting back to Wide-Character string\n");
        len = mbstowcs(pwcs, pmb, MB_CUR_MAX);
        printf("Characters converted %d\n", len);
        printf("Hex value of first wide character %#.4x\n\n", pwcs);
        free(pwcs);
    }

    free(pmb);
    return (0);
}