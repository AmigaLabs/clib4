#include <stdio.h>
#include <string.h>

static const char *__attribute__((used)) stack = "$STACK: 100000\0";

int main() {
    char str[8192 * 10] = {0};

    fgets(str, 5, stdin); // read from stdin
    puts(str); // print read content out to stdout

    // open the file
    FILE *f = fopen("test.txt", "r");
    FILE *fout = fopen("test1.txt", "w");
    // if there was an error
    if (f == NULL) {
        perror("Error opening file"); // print error
        return (-1);
    }
    if (fout == NULL) {
        fclose(f);
        perror("Error opening file"); // print error
        return (-1);
    }        // if there was no error
    else {
        fgets(str, 8192 * 10, f); // read from file
        fwrite(str, 1, strlen(str), fout); // print read content out to stdout
    }

    fclose(f); // close file
    fclose(fout); // close file

    return (0);
}