/* swscanf example */
#include <wchar.h>

int main() {
    wchar_t *sentence = L"Michael is 10 years old";
    wchar_t str[20];
    int i;

    swscanf(sentence, L"%ls %*s %d", str, &i);
    wprintf(L"%ls -> %d\n", str, i);

    return 0;
}