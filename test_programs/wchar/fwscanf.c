/* fwscanf example */
#include <stdio.h>
#include <wchar.h>

int main() {
    wchar_t str[80];
    float f;
    FILE *pFile;

    pFile = fopen("myfile.txt", "w+");
    fwprintf(pFile, L"%f %ls", 3.1416, L"PI");
    rewind(pFile);
    fwscanf(pFile, L"%f", &f);
    fwscanf(pFile, L"%ls", str);
    fclose(pFile);
    wprintf(L"I have read: %f and %ls \n", f, str);
    return 0;
}