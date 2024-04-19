#include "tcpS.h"
#include <pthread.h>

// gcc thread_tcpS.c utils_tcp.c
// ./srv.out 5000

void *ThreadMain(void *arg);

struct ThreadArgs
{
    int clntSock;
};

int main(int argc, char *argv[])
{
    int servSock;
    int clntSock;
    unsigned short echoServPort;
    pthread_t threadID;
    struct ThreadArgs *threadArgs;

    if ((argc != 2)) {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);

    servSock = CreateTCPServerSocket(echoServPort);

    for (;;) {

        clntSock = AcceptTCPConnection(servSock);
        
        if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL)
            DieWithError("malloc() failed.");
        threadArgs->clntSock = clntSock;

        // create client thread
        if (pthread_create(&threadID, NULL, ThreadMain, (void *) threadArgs) != 0)
            DieWithError("pthread_create() failed.");
        printf("with thread 0x%lx\n", (long int) threadID);



    }

}


void *ThreadMain(void *threadArgs)
{
    int clntSock;

    pthread_detach(pthread_self());

    clntSock = ((struct ThreadArgs *) threadArgs)->clntSock;
    free(threadArgs);

    HandleTCPClient(clntSock);

    return (NULL);
}




