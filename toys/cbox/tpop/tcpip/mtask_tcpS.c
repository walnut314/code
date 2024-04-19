#include "tcpS.h"
#include <sys/wait.h>

// gcc mtask_tcpS.c utils_tcp.c
// ./srv.out 5000

int main(int argc, char *argv[])
{
    int servSock;
    int clntSock;
    unsigned short echoServPort;
    pid_t processID;
    unsigned int childProcCount = 0;

    if ((argc != 2)) {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);

    servSock = CreateTCPServerSocket(echoServPort);

    for (;;) {

        clntSock = AcceptTCPConnection(servSock);
        // fork shild process and report errors
        if ((processID = fork()) < 0)
            DieWithError("fork() failed.");

        else if (processID == 0) { // child process
            close(servSock);
            HandleTCPClient(clntSock);
            close(clntSock);
            exit(0);
        }

        printf("with child process %d\n", (int) processID);
        close(clntSock);
        childProcCount++;

        while (childProcCount) {
            processID = waitpid((pid_t) -1, NULL, WNOHANG); // non-block wait
            if (processID < 0)
                DieWithError("waitpid() failed.");
            else if (processID == 0) // no zombie
                break;
            else
                childProcCount--;
        }

    }

}
