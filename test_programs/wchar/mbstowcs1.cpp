#include <iostream>
#include <clocale>
#include <cstdlib>

int main() {
#ifndef __amigaos4__
    std::setlocale(LC_ALL, "en_US.utf8");
    std::wcout.imbue(std::locale("en_US.utf8"));
#endif
    const char *mbstr = "z\u00df\u6c34\U0001f34c"; // or u8"zß水🍌"
    // or "\x7a\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c";
    wchar_t wstr[5];
    std::mbstowcs(wstr, mbstr, 5);
    std::wcout << "wide string: " << wstr << '\n';
}