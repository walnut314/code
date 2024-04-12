#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RCVBUFSIZE  (32)

// usage: ./cli.out 127.0.0.1 "dude" 5000

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;
    unsigned int echoStringLen;
    int rcvBufferSize;
    int sockOptSize;

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket failed");


    sockOptSize = sizeof(rcvBufferSize);
    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvBufferSize, &sockOptSize) < 0)
        DieWithError("getsockopt() failed 1");

    printf("initial rcv buffer: %d\n", rcvBufferSize);
    rcvBufferSize *= 2;
    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &rcvBufferSize, sizeof(rcvBufferSize)) < 0)
        DieWithError("getsockopt() failed 2");
    printf("final rcv buffer: %d\n", rcvBufferSize);


    close(sock);
    exit(0);

}
