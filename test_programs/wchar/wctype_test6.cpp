// An example C++ program to demonstrate use
// of wcscat()
#include <iostream>
#include <cwchar>
using namespace std;

int main()
{
    wchar_t string1[] = L"geeksforgeeks";
    wchar_t string2[] = L" is for Geeks";

    wcscat(string1, string2);

    wcout << L"Concatenated wide string is = "
          << string1 << endl;

    return 0;
}
