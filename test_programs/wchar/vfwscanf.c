#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include <locale.h>

/* Create a README file with a string, a space and a number and execute this test */

void ReadWideStuff(FILE *stream, const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
    vfwscanf(stream, format, args);
    va_end(args);
}

int main()
{
    FILE *pFile;
    int val;
    wchar_t str[100] = {0};

    /* You can also create an UTF 8 file and it will work as well */
    setlocale(LC_ALL, "C-UTF-8");
    pFile = fopen("README", "r");

    if (pFile != NULL)
    {
        ReadWideStuff(pFile, L" %ls %d ", str, &val);
        wprintf(L"I have read %ls and %d\n", str, val);
        fclose(pFile);
    }

    return 0;
}