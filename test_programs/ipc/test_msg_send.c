#include <stdio.h>
#include <sys/shm.h>
#include <sys/msg.h>

int main(void)
{
	int id;

	struct
	{
		long type;
		long data;
	} mymsg;
	int i;

	id = msgget(0x12345, 0777 | IPC_CREAT);
	if (id >= 0)
	{
		mymsg.type = 1;
		mymsg.data = 4711;
		i = msgsnd(id, &mymsg, sizeof(long), 0);
		printf("Send one message. i=%d\n", i);
		mymsg.type = 7;
		mymsg.data = 1701;
		i = msgsnd(id, &mymsg, sizeof(long), 0);
		printf("Send one message. i=%d\n", i);
	}
	else
	{
		printf("No queue!\n");
	}

	return (0);
}
