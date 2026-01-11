#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024

int main(void) {
	char buffer[BUF_SIZE];

	printf("Initial output.\n");
	printf("This line is in the default buffer, but will be flushed at the end.\n");

	// Use setvbuf to assign a new, fully buffered buffer to stdout.
	if (setvbuf(stdout, buffer, _IOFBF, BUF_SIZE) != 0) {
		perror("setvbuf failed");
		return 1;
	}

	printf("This text is now being written to the new custom buffer.\n");
	printf("You will not see this until the buffer is full, or the program ends.\n");

	// We can prove the custom buffer is used by forcing a flush.
	// Uncomment the next line to see the output immediately.
	fflush(stdout);

	printf("Final line before exit.\n");

	return 0;
}