#include "tcpS.h"
#include <sys/wait.h>

// gcc forkN_tcpS.c utils_tcp.c
// ./srv.out 5000

void ProcessMain(int servSock);

int main(int argc, char *argv[])
{
    int servSock;
    unsigned short echoServPort;
    pid_t processID;
    unsigned int processLimit;
    unsigned int processCt;

    if ((argc != 3)) {
        fprintf(stderr, "Usage: %s <Server Port> <fork limit>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);
    processLimit = atoi(argv[2]);

    servSock = CreateTCPServerSocket(echoServPort);

    for (processCt = 1; processCt < processLimit; processCt++) {

        if ((processID = fork()) < 0) 
            DieWithError("fork() failed.");
        else if (processID == 0) // child process
            ProcessMain(servSock);

    }

    exit(0);

}

void ProcessMain(int servSock)
{
    int clntSock;
    static int num = 0;

    printf("dude: %d\n", num);
    num++;

    for (;;) {
        clntSock = AcceptTCPConnection(servSock);
        printf("with child process: %d\n", (unsigned int) getpid());
        HandleTCPClient(clntSock);
    }
}



