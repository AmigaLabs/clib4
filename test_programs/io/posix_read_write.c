#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("test.c", O_RDONLY);
    char buffer[1024];
    ssize_t bytes;

    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes);
    }

    close(fd);
    return 0;
}
