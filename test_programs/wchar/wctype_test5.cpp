// An example C++ program to demonstrate use
// of wcscpy()
#include <iostream>
#include <cwchar>
using namespace std;

int main()
{
    wchar_t waname[] = L"geeksforgeeks";
    wchar_t wacopy[14];
    wcscpy(wacopy, waname);
    wcout << L"Original = " << waname
          << L"\nCopy =  " << wacopy << endl;

    return 0;
}
