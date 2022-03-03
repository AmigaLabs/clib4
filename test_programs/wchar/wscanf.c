/* wscanf example */
#include <wchar.h>

int main() {
    wchar_t str[80];
    int i;

    wprintf(L"Enter your family name: ");
    wscanf(L"%ls", str);
    wprintf(L"Enter your age: ");
    wscanf(L"%d", &i);
    wprintf(L"Mr. %ls, %d years old.\n", str, i);
    wprintf(L"Enter a hexadecimal number: ");
    wscanf(L"%x", &i);
    wprintf(L"You have entered %#x (%d).\n", i, i);

    return 0;
}