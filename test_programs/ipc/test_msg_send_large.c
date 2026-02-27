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
	mymsg[0] = 2501;
	for (i = 1; i <= 128; i++)
	{
		mymsg[i] = i;
	}

	id = msgget(0x12345, 0777 | IPC_CREAT);
	if (id >= 0)
	{
		i = msgsnd(id, mymsg, 512, 0);
		printf("Send one message. i=%d\n", i);
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
