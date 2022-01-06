// An example C++ program to demonstrate use
// of wchar_t in array
#include <iostream>
#include <cwchar>
using namespace std;

int main()
{
    // char type array string
    char caname[] = "geeksforgeeks";
    cout << caname << endl;

    // wide-char type array string
    wchar_t waname[] = L"geeksforgeeks";
    wcout << waname << endl;

    return 0;
}