#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ECHOMAX     (255)

// usage: ./cli.out 127.0.0.1 "dude" 5000

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in fromAddr;
    unsigned short echoServPort;
    unsigned int fromSize;
    char *servIP;
    char *echoString;
    char echoBuffer[ECHOMAX+1];
    int echoStringLen;
    int respStringLen;

    if ((argc < 3) || (argc > 4)) {
        fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
        exit(0);
    }

    servIP = argv[1];
    echoString = argv[2];

    if ((echoStringLen = strlen(echoString)) > ECHOMAX)
        DieWithError("Echo word too long.");

    if (argc == 4)
        echoServPort = atoi(argv[3]);
    else
        echoServPort = 7;

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket failed");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port = htons(echoServPort);

    if (sendto(sock, echoString, echoStringLen, 0, 
                (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) 
            != echoStringLen)
        DieWithError("sendto() sent a different number of bytes than expected.");

    fromSize = sizeof(fromAddr);
    if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0,
                    (struct sockaddr *) &fromAddr,  &fromSize))
            != echoStringLen)
        DieWithError("recvfrom() failed.");

    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
        fprintf(stderr, "Error: received a packet from an unknown source.\n");
        exit(1);
    }

    echoBuffer[respStringLen] = '\0';
    printf("received: %s\n", echoBuffer);

    close(sock);
    exit(0);

}
