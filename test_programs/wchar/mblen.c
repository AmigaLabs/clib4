#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
    int len;
    char *pmbnull  = NULL;
    char *pmb = (char *)malloc( MB_CUR_MAX );
    wchar_t *pwc = L"Hi";
    wchar_t *pwcs = (wchar_t *)malloc( sizeof( wchar_t ));

    printf("Converting to multibyte string\n");
    len = wcstombs( pmb, pwc, MB_CUR_MAX);
    printf("Characters converted %d\n", len);
    printf("Hex value of first multibyte character: %#.4x\n", pmb);

    len = mblen( pmb, MB_CUR_MAX );
    printf( "Length in bytes of multibyte character %x: %u\n", pmb, len );

    pmb = NULL;

    len = mblen( pmb, MB_CUR_MAX );
    printf( "Length in bytes of multibyte character %x: %u\n", pmb, len );

    return(0);
}
