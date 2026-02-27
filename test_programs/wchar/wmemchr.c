/* wmemchr example */
#include <wchar.h>

int main() {
    wchar_t *pwc;
    wchar_t *wcs = L"Example wide string";
    pwc = wmemchr(wcs, L'p', wcslen(wcs));
    if (pwc != NULL)
        wprintf(L"'p' found at position %d.\n", pwc - wcs + 1);
    else
        wprintf(L"'p' not found.\n");
    return 0;
}