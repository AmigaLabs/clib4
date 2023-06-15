#include <stdio.h>
#include <stdarg.h>

void myPrint(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vdprintf(1, format, args);
    va_end(args);
}

int main() {
    int number = 42;
    float pi = 3.14159;
    char message[] = "Hello, world!";

    myPrint("Number: %d\n", number);
    myPrint("Pi: %f\n", pi);
    myPrint("Message: %s\n", message);

    return 0;
}