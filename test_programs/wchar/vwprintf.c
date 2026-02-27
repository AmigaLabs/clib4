/* vwprintf example */
#include <stdarg.h>
#include <wchar.h>

void WriteWideFormatted(const wchar_t *format, ...) {
    va_list args;
    va_start(args, format);
    vwprintf(format, args);
    va_end(args);
}

int main() {
    WriteWideFormatted(L"Call with %d variable argument.\n", 1);
    WriteWideFormatted(L"Call with %d variable %ls.\n", 2, L"arguments");

    return 0;
}