#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/msg.h>

int main(void)
{
	int id;
	long *mymsg = 0;
	int i;

	mymsg = malloc(512 + 4);

	id = msgget(0x12345, 0);
	if (id >= 0)
	{
		i = msgrcv(id, mymsg, 512, 2501, 0);
		printf("Read one message, i=%d\n", i);
		for (i = 1; i <= 128; i++)
		{
			if (mymsg[i] != i)
			{
				printf("Data error @ %d!\n", i);
				break;
			}
		}
	}
	else
	{
		printf("No queue!\n");
	}

	if (mymsg)
	{
		free(mymsg);
	}
	return (0);
}
