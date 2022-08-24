#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: copyfile <from> <to>\n");
        exit(1);
    }

    int ch;
    FILE *source, *target;

    source = fopen(argv[1], "r");
    if (source == NULL) {
        printf("Error opening in file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    target = fopen(argv[2], "w");
    if (target == NULL) {
        fclose(source);
        printf("Error opening out file %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    while ((ch = fgetc(source)) != EOF)
        fputc(ch, target);

    printf("File copied successfully.\n");
    fclose(source);
    fclose(target);

    return 0;
}
