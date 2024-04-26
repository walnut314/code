#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>

unsigned long ResolveName(char *name)
{
    struct hostent *host;
    if ((host = gethostbyname(name)) == NULL) {
        fprintf(stderr, "gethostbyname() failed.\n");
        exit(1);
    }
    return *((unsigned long *) host->h_addr_list[0]);
}

unsigned short ResolveService(char *service, char *protocol)
{
    struct servent *serv;
    unsigned short port;

    if ((port = atoi(service)) == 0) { // port is numeric
        // not numeric
        if ((serv = getservbyname(service, protocol)) == NULL) {
            fprintf(stderr, "getservbyname() failed.\n");
            exit(1);
        } else {
            port = serv->s_port;
        }
    } else {
        port = htons(port);
    }

    return port;
}

int main()
{
    char name[] = "www.intel.com";
    struct in_addr in;
    in.s_addr = ResolveName(name);

    printf("addr: %x %s\n", in.s_addr, inet_ntoa(in));

    int srv = ResolveService("http", "tcp");
    printf("serv port: %x\n", srv);
}


