#include <stdio.h>
#include <ctype.h>

// Function to test isalnum()
void test_isalnum() {
    if (isalnum('a') > 0) {
        printf("isalnum('a') passed\n");
    } else {
        printf("isalnum('a') failed\n");
    }

    if (isalnum('Z') > 0) {
        printf("isalnum('Z') passed\n");
    } else {
        printf("isalnum('Z') failed\n");
    }

    if (isalnum('5') > 0) {
        printf("isalnum('5') passed\n");
    } else {
        printf("isalnum('5') failed\n");
    }

    if (isalnum('@') == 0) {
        printf("isalnum('@') passed\n");
    } else {
        printf("isalnum('@') failed\n");
    }

    if (isalnum(' ') == 0) {
        printf("isalnum(' ') passed\n");
    } else {
        printf("isalnum(' ') failed\n");
    }
}

// Function to test isalpha()
void test_isalpha() {
    if (isalpha('a') > 0) {
        printf("isalpha('a') passed\n");
    } else {
        printf("isalpha('a') failed\n");
    }

    if (isalpha('Z') > 0) {
        printf("isalpha('Z') passed\n");
    } else {
        printf("isalpha('Z') failed\n");
    }

    if (isalpha('1') == 0) {
        printf("isalpha('1') passed\n");
    } else {
        printf("isalpha('1') failed\n");
    }

    if (isalpha(' ') == 0) {
        printf("isalpha(' ') passed\n");
    } else {
        printf("isalpha(' ') failed\n");
    }
}

// Function to test isdigit()
void test_isdigit() {
    if (isdigit('5') > 0) {
        printf("isdigit('5') passed\n");
    } else {
        printf("isdigit('5') failed\n");
    }

    if (isdigit('a') == 0) {
        printf("isdigit('a') passed\n");
    } else {
        printf("isdigit('a') failed\n");
    }

    if (isdigit(' ') == 0) {
        printf("isdigit(' ') passed\n");
    } else {
        printf("isdigit(' ') failed\n");
    }
}

// Function to test islower()
void test_islower() {
    if (islower('a') > 0) {
        printf("islower('a') passed\n");
    } else {
        printf("islower('a') failed\n");
    }

    if (islower('Z') == 0) {
        printf("islower('Z') passed\n");
    } else {
        printf("islower('Z') failed\n");
    }

    if (islower('5') == 0) {
        printf("islower('5') passed\n");
    } else {
        printf("islower('5') failed\n");
    }

    if (islower(' ') == 0) {
        printf("islower(' ') passed\n");
    } else {
        printf("islower(' ') failed\n");
    }
}

// Function to test isupper()
void test_isupper() {
    if (isupper('A') > 0) {
        printf("isupper('A') passed\n");
    } else {
        printf("isupper('A') failed\n");
    }

    if (isupper('z') == 0) {
        printf("isupper('z') passed\n");
    } else {
        printf("isupper('z') failed\n");
    }

    if (isupper('5') == 0) {
        printf("isupper('5') passed\n");
    } else {
        printf("isupper('5') failed\n");
    }

    if (isupper(' ') == 0) {
        printf("isupper(' ') passed\n");
    } else {
        printf("isupper(' ') failed\n");
    }
}

// Function to test isprint()
void test_isprint() {
    if (isprint('a') > 0) {
        printf("isprint('a') passed\n");
    } else {
        printf("isprint('a') failed\n");
    }

    if (isprint(' ') > 0) {
        printf("isprint(' ') passed\n");
    } else {
        printf("isprint(' ') failed\n");
    }

    if (isprint('\n') == 0) {
        printf("isprint('\\n') passed\n");
    } else {
        printf("isprint('\\n') failed\n");
    }

    if (isprint(27) == 0) {
        printf("isprint(27) passed\n");
    } else {
        printf("isprint(27) failed\n");
    }
}

// Function to test isspace()
void test_isspace() {
    if (isspace(' ') > 0) {
        printf("isspace(' ') passed\n");
    } else {
        printf("isspace(' ') failed\n");
    }

    if (isspace('\t') > 0) {
        printf("isspace('\\t') passed\n");
    } else {
        printf("isspace('\\t') failed\n");
    }

    if (isspace('a') == 0) {
        printf("isspace('a') passed\n");
    } else {
        printf("isspace('a') failed\n");
    }

    if (isspace('Z') == 0) {
        printf("isspace('Z') passed\n");
    } else {
        printf("isspace('Z') failed\n");
    }
}

// Function to test isxdigit()
void test_isxdigit() {
    if (isxdigit('0') > 0) {
        printf("isxdigit('0') passed\n");
    } else {
        printf("isxdigit('0') failed\n");
    }

    if (isxdigit('a') > 0) {
        printf("isxdigit('a') passed\n");
    } else {
        printf("isxdigit('a') failed\n");
    }

    if (isxdigit('F') > 0) {
        printf("isxdigit('F') passed\n");
    } else {
        printf("isxdigit('F') failed\n");
    }

    if (isxdigit('G') == 0) {
        printf("isxdigit('G') passed\n");
    } else {
        printf("isxdigit('G') failed\n");
    }

    if (isxdigit(' ') == 0) {
        printf("isxdigit(' ') passed\n");
    } else {
        printf("isxdigit(' ') failed\n");
    }
}

// Function to test tolower()
void test_tolower() {
    if (tolower('A') == 'a') {
        printf("tolower('A') passed\n");
    } else {
        printf("tolower('A') failed\n");
    }

    if (tolower('z') == 'z') {
        printf("tolower('z') passed\n");
    } else {
        printf("tolower('z') failed\n");
    }

    if (tolower('5') == '5') {
        printf("tolower('5') passed\n");
    } else {
        printf("tolower('5') failed\n");
    }

    if (tolower(' ') == ' ') {
        printf("tolower(' ') passed\n");
    } else {
        printf("tolower(' ') failed\n");
    }
}

// Function to test toupper()
void test_toupper() {
    if (toupper('a') == 'A') {
        printf("toupper('a') passed\n");
    } else {
        printf("toupper('a') failed\n");
    }

    if (toupper('Z') == 'Z') {
        printf("toupper('Z') passed\n");
    } else {
        printf("toupper('Z') failed\n");
    }

    if (toupper('5') == '5') {
        printf("toupper('5') passed\n");
    } else {
        printf("toupper('5') failed\n");
    }

    if (toupper(' ') == ' ') {
        printf("toupper(' ') passed\n");
    } else {
        printf("toupper(' ') failed\n");
    }
}

int main() {
    // Run all the test cases
    test_isalnum();
    test_isalpha();
    test_isdigit();
    test_islower();
    test_isupper();
    test_isprint();
    test_isspace();
    test_isxdigit();
    test_tolower();
    test_toupper();
    
    return 0;
}
