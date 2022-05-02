#include <stdio.h>
#include <string.h>
#include <wchar.h>

int
main() {
    const unsigned char buf[] = {'a', 'b', '\0', 'c', 'd', '\0', 'e'};
    wchar_t out[sizeof(buf)];
    memset(out, '\xff', sizeof(out));

    mbstate_t state;
    memset(&state, '\0', sizeof(state));

    const char *in = (const char *) buf;
    size_t n = mbsrtowcs(out, &in, sizeof(out) / sizeof(wchar_t), &state);

    int result = 0;
    if (n != 2) {
        printf("n = %zu, expected 2\n", n);
        result = 1;
    }
    if (in != NULL) {
        printf("in = %p, expected NULL\n", in);
        result = 1;
    }
    if (out[0] != L'a') {
        printf("out[0] = L'%lc', expected L'a'\n", (wint_t) out[0]);
        result = 1;
    }
    if (out[1] != L'b') {
        printf("out[1] = L'%lc', expected L'b'\n", (wint_t) out[1]);
        result = 1;
    }
    if (out[2] != L'\0') {
        printf("out[2] = L'%lc', expected L'\\0'\n", (wint_t) out[2]);
        result = 1;
    }
    if (result == 0)
        printf("All tests ok\n");
    return result;
}