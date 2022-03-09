/* wcstombs example */
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* wcstombs, wchar_t(C) */

int main() {
    const wchar_t str[] = L"wcstombs example";
    char buffer[32];
    int ret;

    wprintf(L"wchar_t string: %ls \n", str);

    ret = wcstombs(buffer, str, sizeof(buffer));
    if (ret == 32) buffer[31] = '\0';
    if (ret) printf("multibyte string: %s \n", buffer);

    return 0;
}