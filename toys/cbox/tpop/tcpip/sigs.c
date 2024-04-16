#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void InterruptSignalHandler(int signalType)
{
    printf("Interrupt received. Exiting Program.\n");

    exit(1);
}

int main(int argc, char *argv[])
{
    struct sigaction handler;

    handler.sa_handler = InterruptSignalHandler;
    if (sigfillset(&handler.sa_mask) < 0)
        DieWithError("sigfillset() failed.");

    handler.sa_flags = 0;

    if (sigaction(SIGINT, &handler, 0) < 0)
        DieWithError("sigaction() failed.");

    for (;;)
        pause();

    exit(0);
}
