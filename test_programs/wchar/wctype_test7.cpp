// An example C++ program to demonstrate use
// of wcscmp()
#include <iostream>
#include <cwchar>
using namespace std;

int main()
{
    wchar_t string1[] = L"geeksforgeeks";
    wchar_t string2[] = L"GEEKS";
    wcout << L"Comparison1 = "
          << wcscmp(string1, string2) << endl;
    wcout << L"Comparison2 = "
          << wcscmp(string1, string1) << endl;
    wcout << L"Comparison3 = "
          << wcscmp(string2, string1) << endl;
    return 0;
}
