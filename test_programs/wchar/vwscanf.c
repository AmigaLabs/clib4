/* vwscanf example */
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

void GetWideMatches(const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    vwscanf(format, args);
    va_end(args);
}

int main() {
    int val;
    wchar_t str[100];

    wprintf(L"Please enter a number and a word: ");
    GetWideMatches(L"%d %l[aeiou]", &val, str);
    wprintf(L"Number read: %d\nFirst vowels read: %ls\n", val, str);

    return 0;
}