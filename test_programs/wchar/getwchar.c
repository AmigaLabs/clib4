/* getwchar example */
#include <stdio.h>
#include <wchar.h>

int main() {
    wint_t wc;
    fputws(L"Enter text. Include a dot (.) in a sentence to exit:\n", stdout);
    do {
        wc = getwchar();
        putwchar(wc);
    } while (wc != L'.');
    return 0;
}