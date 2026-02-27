/* ungetwc example */
#include <stdio.h>
#include <wchar.h>

int main() {
    FILE *pFile;
    wint_t wc;
    wchar_t buffer[256];

    pFile = fopen("myfile.txt", "rt");
    if (pFile != NULL)
        while (!feof(pFile)) {
            wc = getwc(pFile);
            if (wc != WEOF) {
                if (wc == L'#') ungetwc(L'@', pFile);
                else ungetwc(wc, pFile);
                fgetws(buffer, 255, pFile);
                fputws(buffer, stdout);
            }
        }
    return 0;
}