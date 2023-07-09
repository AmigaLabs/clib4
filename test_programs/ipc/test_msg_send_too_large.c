#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/msg.h>

int main(void) {
    int id;
    long *mymsg = 0;
    int i;

    mymsg = malloc(32768 + 4);
    mymsg[0] = 666;

    id = msgget(0x12345, 0777 | IPC_CREAT);
    if (id >= 0) {
        i = msgsnd(id, mymsg, 32768 + 4, 0);
        printf("Send one message. i=%d\n", i);
    } else {
        printf("No queue!\n");
    }

    if (mymsg) {
        free(mymsg);
    }

    return (0);
}
