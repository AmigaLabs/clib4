#include <stdio.h>
#include <wchar.h>

int main() {
    const char output[256] = {0};
    const wchar_t *wc = L"Hellow World";
    sprintf(output, "%ws", wc);
    printf("output = %s\n", output);

    return 0;
}