#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("Executable full name: %s\n", getexecfullname());
    return 0;
}