#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    struct dirent **namelist;
    int i, n;

    n = scandir("T:", &namelist, 0, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        for (i = 0; i < n; i++) {
            printf("%s\n", namelist[i]->d_name);
            free(namelist[i]);
        }
    }
    free(namelist);

    return 0;
}