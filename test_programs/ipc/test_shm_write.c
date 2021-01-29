#include <stdio.h>
#include <sys/shm.h>

int main(void)
{
	int id;
	int *mem = 0;
	int i;

	id = shmget(0xabcde, 4096, 0777 | IPC_CREAT);
	if (id >= 0)
	{
		mem = shmat(id, 0, 0);
		if (mem)
		{
			for (i = 0; i < 32; i++)
			{
				mem[i] = i;
			}
			printf("Wrote 32 integers to memory region.\n");
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
