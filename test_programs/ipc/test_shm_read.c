#include <stdio.h>
#include <sys/shm.h>

int main(void)
{
	int id;
	int *mem = 0;
	int i;

	id = shmget(0xabcde, 4096, 0);
	if (id >= 0)
	{
		mem = shmat(id, 0, 0);
		if (mem)
		{
			for (i = 0; i < 32; i++)
			{
				if (mem[i] != i)
				{
					printf("Mismatch @ index %d.\n", i);
				}
			}
			printf("Read 32 integers from memory region.\n");
			shmdt(mem);
		}
		else
		{
			printf("shmat() failed.\n");
		}
	}
	else
	{
		printf("No queue!\n");
	}

	return (0);
}
