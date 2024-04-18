#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>


#define ECHOMAX         (255)
#define TIMEOUT_SECS      (2)
#define MAXTRIES          (5)

int tries = 0;

void DieWithError(char *errorMessage);
void CatchAlarm(int ignored);

// usage: ./cli.out 127.0.0.1 "dude" 5000

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void CatchAlarm(int ignored)
{
    tries += 1;
}

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in fromAddr;
    unsigned short echoServPort;
    unsigned int fromSize;
    struct sigaction myAction;
    char *servIP;
    char *echoString;
    char echoBuffer[ECHOMAX+1];
    int echoStringLen;
    int respStringLen;

    if ((argc < 3) || (argc > 4)) {
        fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
        exit(1);
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

    myAction.sa_handler = CatchAlarm;
    if (sigfillset(&myAction.sa_mask) < 0)
        DieWithError("sigfillset() failed.");
    myAction.sa_flags = 0;

    if (sigaction(SIGALRM, &myAction, 0) < 0)
        DieWithError("sigaction() failed.");

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port = htons(echoServPort);

    if (sendto(sock, echoString, echoStringLen, 0, 
                (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) 
            != echoStringLen)
        DieWithError("sendto() sent a different number of bytes than expected.");

    /* get the response */
    fromSize = sizeof(fromAddr);
    alarm(TIMEOUT_SECS);
    while ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0,
                    (struct sockaddr *) &fromAddr,  &fromSize)) < 0)
        if (errno == EINTR) {
            if (tries < MAXTRIES) {
                printf("timed out, %d more tries...\n", MAXTRIES-tries);
                if (sendto(sock, echoString, echoStringLen, 0, 
                            (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
                    DieWithError("sendto() failed.");
                alarm(TIMEOUT_SECS);
            } else 
                DieWithError("No response.");
        } else 
            DieWithError("recvfrom failed.");

    /* recvfrom() got something */
    alarm(0);

    echoBuffer[respStringLen] = '\0';
    printf("received: %s\n", echoBuffer);

    close(sock);
    exit(0);

}
