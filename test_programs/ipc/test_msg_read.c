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

	id = msgget(0x12345, 0);
	if (id >= 0)
	{
		i = msgrcv(id, &mymsg, sizeof(long), 7, 0);
		printf("Recieved one message. i=%d, type=%d, data=%d\n", i, mymsg.type, mymsg.data);
		i = msgrcv(id, &mymsg, sizeof(long), 1, 0);
		printf("Recieved one message. i=%d, type=%d, data=%d\n", i, mymsg.type, mymsg.data);
	}
	else
	{
		printf("No queue!\n");
	}

	return (0);
}
