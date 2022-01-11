#include <unistd.h>
#include <stdio.h>
#include <glob.h>

int main(void)
{
    glob_t paths;
    int retval;

    paths.gl_pathc = 0;
    paths.gl_pathv = NULL;
    paths.gl_offs = 0;

    retval = glob("*.*", GLOB_NOCHECK | GLOB_NOSORT, NULL, &paths);
    if (retval == 0)
    {
        int idx;

        for (idx = 0; idx < paths.gl_pathc; idx++)
        {
            printf("[%d]: %s\n", idx, paths.gl_pathv[idx]);
        }

        globfree(&paths);
    }
    else
    {
        puts("glob() failed");
    }

    return 0;
}