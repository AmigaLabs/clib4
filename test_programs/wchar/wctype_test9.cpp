// An example C++ program to demonstrate use
// of wcsncpy()
#include <iostream>
#include <cwchar>
using namespace std;

int main()
{
    wchar_t string1[] = L"Geeks For Geeks";
    wchar_t string2[20];
    wchar_t string3[20];

    wcsncpy(string2, string1, 20);

    // partial copy
    wcsncpy(string3, string2, 5);

    string3[5] = L'\0'; // null character manually added

    wcout << string1 << endl
          << string2
          << endl
          << string3
          << endl;

    return 0;
}
