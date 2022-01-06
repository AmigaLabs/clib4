// An example C++ program to demonstrate use
// of wcslen()
#include <iostream>
#include <cwchar>
using namespace std;

int main()
{
    // wide-char type array string
    wchar_t waname[] = L"geeksforgeeks";

    wcout << L"The length of '" << waname
          << L"' is " << wcslen(waname) << endl;

    return 0;
}
