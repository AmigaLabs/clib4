#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

int main(void)
{
	int id;
	struct sembuf sop = {
		.sem_num = 0,
		.sem_op = -1,
		.sem_flg = 0};
	int i, n;
	struct semid_ds si;

	id = semget(0xF00D, 1, 0777);
	if (id >= 0)
	{
		semctl(id, 0, IPC_STAT, (union semun){.buf = &si});
		for (i = 0; i < si.sem_nsems; i++)
		{
			n = semctl(id, i, GETVAL, (union semun){0});
			printf("sem[%d]=%d\n", i, n);
		}
		printf("Ok.\n");
	}
	else
	{
		printf("No array!\n");
	}

	return (0);
}
