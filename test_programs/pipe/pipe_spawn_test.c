#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h> /* spawnvpe */

/* Child mode: read stdin and echo to stdout with a prefix. */
static int run_child(void)
{
	char buf[256];
	int total = 0;
	while (fgets(buf, sizeof(buf), stdin)) {
		total += (int)strlen(buf);
		printf("[child] got: %s", buf);
	}
	printf("[child] eof, total=%d\n", total);
	return 0;
}

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "child") == 0)
		return run_child();

	/* Parent mode */
	int fd[2];
	if (pipe(fd) < 0) {
		perror("pipe");
		return 1;
	}

	int use_dup = (argc > 1 && strcmp(argv[1], "-dup") == 0);

	int fhin = fd[0];
	int fhout = 1;
	int fherr = 2;
	int saved0 = -1;

	if (use_dup && fhin > 0) {
		saved0 = dup(0);
		if (saved0 < 0) {
			perror("dup saved0");
			return 1;
		}
		if (dup2(fhin, 0) < 0) {
			perror("dup2 to stdin");
			return 1;
		}
		fhin = 0;
	}

	const char *child_argv[] = { argv[0], "child", NULL };
	extern char **environ;

	printf("[parent] spawning child use_dup=%d fhin=%d fhout=%d fherr=%d\n",
	       use_dup, fhin, fhout, fherr);

	pid_t pid = spawnvpe(child_argv[0], child_argv, environ, NULL, fhin, fhout, fherr);
	int saved_errno = errno;

	if (use_dup && saved0 >= 0) {
		dup2(saved0, 0);
		close(saved0);
	}

	if (pid < 0) {
		errno = saved_errno;
		perror("spawnvpe");
		return 1;
	}

	/* Close read end in parent; write data then close write end. */
	close(fd[0]);
	const char *msg = "one\ntwo\nthree\n";
	ssize_t w = write(fd[1], msg, strlen(msg));
	if (w < 0)
		perror("write");
	close(fd[1]);

	int status;
	if (waitpid(pid, &status, 0) < 0) {
		perror("waitpid");
		return 1;
	}
	printf("[parent] child exit status=%d\n", status);
	return 0;
}
