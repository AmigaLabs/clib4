#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>

int main(void) {
    int id;
    struct sembuf sop = {
            .sem_num = 0,
            .sem_op = 1,
            .sem_flg = SEM_UNDO};
    int i;

    id = semget(0xF00D, 1, 0777 | IPC_CREAT);
    if (id >= 0) {
        printf("id=%d\n", id);
        semop(id, &sop, 1);
        printf("Ok, posted 1 - now sleeping a while.");
        for (i = 0; i < 10; i++) {
            sleep(1);
            printf(".");
            fflush(stdout);
        }
        printf("\n");
    } else {
        printf("No array!\n");
    }

    return (0);
}
