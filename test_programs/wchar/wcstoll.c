/* wcstoll example */
#include <wchar.h>

int main() {
    wchar_t *wsNumbers = L"1856892505 17b00a12b -01100011010110000010001101100 0x6fffff";
    wchar_t *pEnd;
    long long int lli1, lli2, lli3, lli4;
    lli1 = wcstoll(wsNumbers, &pEnd, 10);
    lli2 = wcstoll(pEnd, &pEnd, 16);
    lli3 = wcstoll(pEnd, &pEnd, 2);
    lli4 = wcstoll(pEnd, NULL, 0);
    wprintf(L"The decimal equivalents are: %lld, %lld, %lld and %lld.\n", lli1, lli2, lli3, lli4);
    return 0;
}