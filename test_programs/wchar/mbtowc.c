#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
    char *str = "This is tutorialspoint.com";
    wchar_t mb[100] = {0};
    int len;

    len = mblen(str, MB_CUR_MAX);
    printf("len = %d\n", len);

    len = mblen(NULL, MB_CUR_MAX);
    printf("len = %d\n", len);
    printf("len*strlen(str) = %ld\n", len*strlen(str));

    mbtowc(mb, str, len*strlen(str) );

    wprintf(L"%ls \n", mb );

    return(0);
}