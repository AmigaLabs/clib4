/* vswscanf example */
#include <stdarg.h>
#include <wchar.h>

void GetWideMatches(const wchar_t *str, const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    vswscanf(str, format, args);
    va_end(args);
}

int main() {
    int val;
    wchar_t buf[100];

    GetWideMatches(L"99 bottles of beer on the wall", L" %d %ls ", &val, buf);

    wprintf(L"Product: %ls\nQuantity: %d\n", buf, val);

    return 0;
}