#include <regex.h>
#include <stdio.h>

// Driver Code
int main() {

    // Variable to create regex
    regex_t regex;

    // Variable to store the return
    // value after creation of regex
    int value;

    // Function call to create regex
    value = regcomp( &regex, "[:word:]", 0);

    // If compilation is successful
    if (value == 0) {
        printf("RegEx compiled successfully.\n");
    }

        // Else for Compilation error
    else {
        printf("Compilation error.\n");
    }
    return 0;
}