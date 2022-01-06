#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <grp.h>

int main()
{
    int fd;
    gid_t grpid;
    struct stat fileStatus;
    struct group *gr_data;
    char words[] = "Test File";

    /* Create a test file. */
    if ((fd = creat("test.file", S_IRUSR | S_IWUSR)) == -1)
    {
        perror("creat error");
        exit(1);
    }
    else
        write(fd, words, strlen(words));

    /* Get group 10 for new group. */
    gr_data = getgrnam("QA");
    if (gr_data == NULL)
    {
        perror("getgrnam error");
        exit(1);
    }
    grpid = gr_data->gr_gid;

    /* Get file status and then print user and group IDs. */
    if (fstat(fd, &fileStatus) != 0)
    {
        perror("fstat error");
        exit(1);
    }
    else
    {
        printf("UID=%d GID=%d n", (int)fileStatus.st_uid, (int)fileStatus.st_gid);
        /* Change file ownership. */
        if (fchown(fd, seteuid(0), grpid) != 0)
            perror("fchown error");
        printf("UID=%d GID=%d n", (int)fileStatus.st_uid,
               (int)fileStatus.st_gid);
    }
    close(fd);
}