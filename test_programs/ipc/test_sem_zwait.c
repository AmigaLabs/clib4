#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

int main(void)
{
	int id;
	struct sembuf sop = {
		.sem_num = 0,
		.sem_op = 0,
		.sem_flg = 0};
	int i;

	id = semget(0xF00D, 1, 0777 | IPC_CREAT);
	if (id >= 0)
	{
		printf("id=%d\n", id);
		semop(id, &sop, 1);
		printf("Ok, now 0.\n");
	}
	else
	{
		printf("No array!\n");
	}

	return (0);
}
