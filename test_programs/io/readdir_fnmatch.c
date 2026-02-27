#include <fnmatch.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

/* On OS4 Console use pattern like **.info */
int main(int argc, char *argv[]) {
    char *pattern;
    DIR *dir;
    struct dirent *entry;
    int ret;
    if (!argv[1] || !argv[2]) {
        printf("Usage readdir_fnmatch PATTERN DIR\n");
        return -1;
    }
    dir = opendir(argv[2]);

    pattern = argv[1];

    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            ret = fnmatch(pattern, entry->d_name, FNM_PATHNAME | FNM_PERIOD);
            if (ret == 0) {
                printf("%s MATCH\n", entry->d_name);
            } else if (ret == FNM_NOMATCH) {
                printf("%s DOESN'T MATCH\n", entry->d_name);
                continue;
            } else {
                printf("error file=%s\n", entry->d_name);
            }
        }
        closedir(dir);
    } else {
        printf("Cannot open dir %s\n", argv[2]);
    }
    return 0;
}
