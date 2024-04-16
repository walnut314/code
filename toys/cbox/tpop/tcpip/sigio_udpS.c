#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <signal.h>
#include <errno.h>

// usage: ./a.out 5000

#define ECHOMAX     (255)

#define RCVBUFSIZE  (32)
#define MAXPENDING  (5)

void DieWithError(char *errorMessage);
void UseIdleTime();
void SIGIOHandler(int signalType);

int sock;

int main(int argc, char *argv[])
{
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort;
    struct sigaction handler;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed.");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(echoServPort);

    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed.");

    handler.sa_handler = SIGIOHandler;
    if (sigfillset(&handler.sa_mask) < 0)
        DieWithError("sigfillset() failed.");
    handler.sa_flags = 0;

    if (sigaction(SIGIO, &handler, 0) < 0)
        DieWithError("sigaction() failed.");

    if (fcntl(sock, F_SETOWN, getpid()) < 0)
        DieWithError("Unable to set process owner to us.");

    // arranage for non-blocking IO
    if (fcntl(sock, F_SETFL, O_NONBLOCK | FASYNC) < 0)
        DieWithError("Unable to put client sock into nonblocking/async mode");

    // go off and do real work
    for (;;)
        UseIdleTime();

}

void UseIdleTime()
{
    printf(".\n");
    sleep(3);
}

void SIGIOHandler(int signalType)
{
    struct sockaddr_in echoClntAddr;
    unsigned int clntLen;
    int recvMsgSize;
    char echoBuffer[ECHOMAX];

    do {

        clntLen = sizeof(echoClntAddr);

        if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
                    (struct sockaddr *) &echoClntAddr, &clntLen)) < 0) {

            if (errno != EWOULDBLOCK)
                DieWithError("recvfrom() failed.");

        } else {

            echoBuffer[recvMsgSize] = '\0';
            printf("Handling client %s, msg %s\n", inet_ntoa(echoClntAddr.sin_addr), echoBuffer);

            if (sendto(sock, echoBuffer, recvMsgSize, 0, (struct sockaddr *) &echoClntAddr,
                            sizeof(echoClntAddr)) != recvMsgSize)
                    DieWithError("sendto() failed.");

        }

    } while (recvMsgSize >= 0);
}


// usage:  ./srv.out 5000

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

#if 0
void RcvMsg(int clntSocket)
{
    msg message;
    void *buffer = &message;
    int rbytes, rv;
    for (rbytes = 0; rbytes < sizeof(message); rbytes += rv) {
        if ((rv = recv(clntSocket, buffer+rbytes, sizeof(message)-rbytes, 0)) <= 0) {
            DieWithError("recv() failed");
        }
    }
    printf("got it!\n");
    printf("dude: %d %lf\n", message.type, message.amt);


    int recvMsgSize = sizeof(message);
    char *echoBuffer = (char *) buffer;
    while (recvMsgSize > 0) {
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            DieWithError("send() failed");
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
            DieWithError("recv() failed");
    }
 


    close(clntSocket);
}

void HandleTCPClient(int clntSocket)
{
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;

    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");
    while (recvMsgSize > 0) {
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            DieWithError("send() failed");
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
            DieWithError("recv() failed");
    }
    close(clntSocket);
}

int main(int argc, char *argv[])
{
    int servSock;
    int clntSock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned short echoServPort;
    unsigned int clntLen;

    if ((argc != 2)) {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);

    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(echoServPort);

    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    for (;;) {
        clntLen = sizeof(echoClntAddr);
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");
        printf("handlingclient %s\n", inet_ntoa(echoClntAddr.sin_addr));
        //HandleTCPClient(clntSock);
        RcvMsg(clntSock);
    }

}

#endif

