#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUFSIZE (100)

#ifndef __amigaos4__
#define sigval_t __sigval_t
#endif

void aio_read_completion_handler(sigval_t sigval);
void aio_write_completion_handler(sigval_t sigval);
struct aiocb my_aiocb1, my_aiocb2;

// Global variables
struct aiocb *cblist[2];
int theSocket;

void InitializeAiocbData(struct aiocb *pAiocb, char *pBuffer) {
    bzero((char *) pAiocb, sizeof(struct aiocb));

    pAiocb->aio_fildes = theSocket;
    pAiocb->aio_nbytes = BUFSIZE;
    pAiocb->aio_offset = 0;
    pAiocb->aio_buf = pBuffer;
}

void IssueReadOperation(struct aiocb *pAiocb, char *pBuffer) {
    InitializeAiocbData(pAiocb, pBuffer);

    /* Link the AIO request with a thread callback */
    pAiocb->aio_sigevent.sigev_notify = SIGEV_THREAD;
    pAiocb->aio_sigevent.sigev_notify_function = aio_read_completion_handler;
    pAiocb->aio_sigevent.sigev_notify_attributes = NULL;
    pAiocb->aio_sigevent.sigev_value.sival_ptr = &my_aiocb1;

    int ret = aio_read(pAiocb);
    assert(ret >= 0);
}

void IssueWriteOperation(struct aiocb *pAiocb, char *pBuffer) {
    InitializeAiocbData(pAiocb, pBuffer);

    /* Link the AIO request with a thread callback */
    pAiocb->aio_sigevent.sigev_notify = SIGEV_THREAD;
    pAiocb->aio_sigevent.sigev_notify_function = aio_write_completion_handler;
    pAiocb->aio_sigevent.sigev_notify_attributes = NULL;
    pAiocb->aio_sigevent.sigev_value.sival_ptr = &my_aiocb2;

    int ret = aio_write(pAiocb);
    assert(ret >= 0);
}

void aio_write_completion_handler(sigval_t sigval) {
    struct aiocb *req;
    req = (struct aiocb *) sigval.sival_ptr;
}

void aio_read_completion_handler(sigval_t sigval) {
    struct aiocb *req;
    req = (struct aiocb *) sigval.sival_ptr;
}

int main() {
    int ret;
    int nPort = 11111;
    char *szServer = "102.168.0.204";

    // Connect to the remote server
    theSocket = socket(AF_INET, SOCK_STREAM, 0);
    assert(theSocket >= 0);

    struct hostent *pServer;
    struct sockaddr_in serv_addr;
    pServer = gethostbyname(szServer);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(nPort);
    bcopy((char *) pServer->h_addr, (char *) &serv_addr.sin_addr.s_addr, pServer->h_length);

    assert(connect(theSocket, (const struct sockaddr *) (&serv_addr), sizeof(serv_addr)) >= 0);

    // Construct the AIO callbacks array
    char *pBuffer = malloc(BUFSIZE + 1);

    bzero((char *) cblist, sizeof(cblist));
    cblist[0] = &my_aiocb1;
    cblist[1] = &my_aiocb2;

    // Start the read and write operations on the same socket
    IssueReadOperation(&my_aiocb1, pBuffer);
    IssueWriteOperation(&my_aiocb2, pBuffer);

    // Wait for I/O completion on both operations
    int nRound = 1;
    printf("\naio_suspend round #%d:\n", nRound++);
    ret = aio_suspend(cblist, 2, NULL);
    assert(ret == 0);

    // Check the error status for the read and write operations
    ret = aio_error(&my_aiocb1);
    assert(ret == EWOULDBLOCK);

    // Get the return code for the read
    {
        ssize_t retcode = aio_return(&my_aiocb1);
        printf("First read operation results: aio_error=%d, aio_return=%d  -  That's the first EWOULDBLOCK\n", ret, retcode);
    }

    ret = aio_error(&my_aiocb2);
    assert(ret == EINPROGRESS);
    printf("Write operation is still \"in progress\"\n");

    // Re-issue the read operation
    IssueReadOperation(&my_aiocb1, pBuffer);

    // Wait for I/O completion on both operations
    printf("\naio_suspend round #%d:\n", nRound++);
    ret = aio_suspend(cblist, 2, NULL);
    assert(ret == 0);

    // Check the error status for the read and write operations for the second time
    ret = aio_error(&my_aiocb1);
    assert(ret == EINPROGRESS);
    printf("Second read operation request is suddenly marked as \"in progress\"\n");

    ret = aio_error(&my_aiocb2);
    assert(ret == 0);

    // Get the return code for the write
    {
        ssize_t retcode = aio_return(&my_aiocb2);
        printf("Write operation has completed with results: aio_error=%d, aio_return=%d\n", ret, retcode);
    }

    // Now try waiting for the read operation to complete - it'll just busy-wait, receiving "EWOULDBLOCK" indefinitely
    do {
        printf("\naio_suspend round #%d:\n", nRound++);
        ret = aio_suspend(cblist, 1, NULL);
        assert(ret == 0);

        // Check the error of the read operation and re-issue if needed
        ret = aio_error(&my_aiocb1);
        if (ret == EWOULDBLOCK) {
            IssueReadOperation(&my_aiocb1, pBuffer);
            printf("EWOULDBLOCK again on the read operation!\n");
        }
    } while (ret == EWOULDBLOCK);

    free(pBuffer);

    return 0;
}