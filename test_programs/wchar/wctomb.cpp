#include <iostream>
#include <iomanip>
#include <clocale>
#include <string>
#include <cstdlib>

void print_wide(const std::wstring& wstr) {
    bool shifts = std::wctomb(nullptr, 0); // reset the conversion state
    std::cout << "shift sequences are " << (shifts ? "" : "not" )
              << " used\n" << std::uppercase << std::setfill('0');
    for (const wchar_t wc : wstr) {
        std::string mb(MB_CUR_MAX, '\0');
        const int ret = std::wctomb(&mb[0], wc);
        const char* s = ret > 1 ? "s" : "";
        std::cout << "multibyte char '" << mb << "' is " << ret
                  << " byte" << s << ": [" << std::hex;
        for (int i{0}; i != ret; ++i) {
            const int c = 0xFF & mb[i];
            std::cout << (i ? " " : "") << std::setw(2) << c;
        }
        std::cout << "]\n" << std::dec;
    }
}

int main() {
    std::setlocale(LC_ALL, "en_US.utf8");
    // UTF-8 narrow multibyte encoding
    std::wstring wstr = L"z\u00df\u6c34\U0001d10b"; // or L"zß水𝄋"
    print_wide(wstr);
}