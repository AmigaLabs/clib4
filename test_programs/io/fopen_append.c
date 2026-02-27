#include <stdio.h>

int main() {
    FILE *pFile;
    FILE *pFile2;
    char buffer[256] = {0};

    pFile = fopen("README", "r");
    pFile2 = fopen("README2", "a");
    if (pFile == NULL) {
        perror("Error opening file.");
    } else {
        while (fgets(buffer, sizeof(buffer), pFile)) {
            fprintf(pFile2, "%s", buffer);
        }
    }
    fclose(pFile);
    fclose(pFile2);

    return 0;
}