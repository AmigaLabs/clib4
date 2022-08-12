#include <stdio.h>
#include <wctype.h>

int main()
{
    int i = 0;
    wchar_t *str = L"Test String.\n";
    wchar_t c;
    wctype_t check = wctype("lower");
    wctrans_t trans = wctrans("toupper");
    while (str[i])
    {
        c = str[i];
        if (iswctype(c, check))
            c = towctrans(c, trans);
        putwchar(c);
        i++;
    }
    return 0;
}