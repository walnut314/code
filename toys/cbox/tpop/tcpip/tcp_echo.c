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

typedef struct _msg {
    int type;
    double amt;
} msg;

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort;
    char *servIP;
    char *echoString;
    char echoBuffer[RCVBUFSIZE];
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;

    if ((argc < 3) || (argc > 4)) {
        fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
        exit(0);
    }

    servIP = argv[1];
    echoString = argv[2];

    if (argc == 4)
        echoServPort = atoi(argv[3]);
    else
        echoServPort = 7;

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port = htons(echoServPort);

    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");

    echoStringLen = strlen(echoString);

    msg message;
    message.type = 42;
    message.amt = 3.141592;
    echoString =(char *)  &message;
    echoStringLen = sizeof(message);

    //if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
    if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
        DieWithError("send() sent a different number of bytes than expected");

    totalBytesRcvd = 0;
    printf("Received: ");
    while (totalBytesRcvd < echoStringLen) {
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE-1, 0)) <= 0)
            DieWithError("recv() failed or connection closed prematurely");
        totalBytesRcvd += bytesRcvd;
        echoBuffer[bytesRcvd] = '\0';
        //printf(echoBuffer);
    }

    msg *m = (msg *) echoBuffer;
    printf("got it back\n");
    printf("wusup: %d %lf\n", m->type, m->amt);


    close(sock);
    exit(0);

}
