// An example C++ program to demonstrate use
// of wcstok()
#include <iostream>
#include <cwchar>
using namespace std;

int main()
{
    wchar_t string[] = L"geeksforgeeks,is,for,GEEKS";

    wchar_t *internal_state;

    wchar_t delim[] = L",";
    wchar_t *token = wcstok(string, delim, &internal_state);

    while (token)
    {
        wcout << token << endl;
        token = wcstok(NULL, delim, &internal_state);
    }

    return 0;
}
