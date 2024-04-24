#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// usage: ./cli.out 127.0.0.1 "dude" 5000

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in broadcastAddr;
    char *broadcastIP;
    unsigned short broadcastPort;
    char *sendString;
    int broadcastPermission;
    unsigned int sendStringLen;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <IP Address> <Port> <Send String>\n", argv[0]);
        exit(1);
    }

    broadcastIP = argv[1];
    broadcastPort = atoi(argv[2]);
    sendString = argv[3];

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed.");

    broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed.");

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);
    broadcastAddr.sin_port = htons(broadcastPort);

    sendStringLen = strlen(sendString);
    for (;;) {
        if (sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *) &broadcastAddr,
                    sizeof(broadcastAddr)) != sendStringLen)
            DieWithError("sendto() sent a different number of bytes expected.");
        sleep(3);
    }

}
