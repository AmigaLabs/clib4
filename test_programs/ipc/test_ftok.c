#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>

int main(int cnt, char *arg[])
{
	int id;
	int i, n;
	key_t key;

	if (cnt == 3)
	{
		i = atoi(arg[2]);
		key = ftok(arg[1], i);
		n = __get_errno();
		printf("ftok(\"%s\",%d) = %08x\n", arg[1], i, key);
		printf("errno=%d\n", n);
	}
	else
	{
		printf("%s <file> <value>\n", arg[0]);
	}

	return (0);
}
