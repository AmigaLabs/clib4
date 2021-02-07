// An example C++ program to demonstrate use
// of wcsstr()
#include <iostream>
#include <cwchar>
using namespace std;

int main()
{
    wchar_t string1[] = L"Geeks Are Geeks";
    wchar_t *string2 = wcsstr(string1, L"Are");
    wcsncpy(string2, L"For", 3);
    wcout << string1 << endl;
    return 0;
}
