#define _GNU_SOURCE  // Enable GNU extensions (including canonicalize_file_name)
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <path>\n", argv[0]);
		return 1;
	}

	char *canon_path = canonicalize_file_name(argv[1]);

	if (canon_path == NULL) {
		perror("Error canonicalizing path");
		return 1;
	}

	printf("Original path: %s\n", argv[1]);
	printf("Canonical path: %s\n", canon_path);

	free(canon_path);  // Must free allocated memory
	return 0;
}