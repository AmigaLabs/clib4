#include <stdio.h>
#include <unistd.h>

int main() {
	int fd[2];
	pipe(fd);

	printf("%d %d\n", isatty(fd[0]), isatty(fd[1]));
	close(fd[0]);
	close(fd[1]);

	return 0;
}