#define _ISOC99_SOURCE

#include <inttypes.h>
#include <stdio.h>

int main(void) {
    wchar_t *nptr;
    wchar_t *endptr;
    uintmax_t j;
    int base = 10;
    nptr = L"10110134932";
    wprintf(L"nptr = `%ls`\n", nptr);
    j = wcstoumax(nptr, &endptr, base);
    wprintf(L"wcstoumax = %ju\n", j);
    wprintf(L"Stopped scan at `%ls`\n\n", endptr);
}