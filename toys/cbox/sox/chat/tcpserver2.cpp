#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "string.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#include "pthread.h"
#include <string>

#include "sox_api.hpp"

// gcc tcpserver2.c -o server2 -pthread 
// ./server 
struct chat_t {
    struct sockaddr_in cl_addr;
    void *socket;
};
void * receiveMessage(void * socket) {
    unsigned long sockfd, ret;
    char buffer[BUF_SIZE]; 
    sockfd = (unsigned long) socket;
    memset(buffer, 0, BUF_SIZE);  
    for (;;) {
        ret = ::recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);  
        if (ret < 0) {  
            printf("Error receiving data!\n");    
        } else {
            printf("client: ");
            fputs(buffer, stdout);
            //printf("\n");
        }  
    }
}
void * sendMessage(void * chat) {
    struct chat_t *ch = (struct chat_t *) chat;
    unsigned long sockfd, ret, len;
    char buffer[BUF_SIZE]; 
    len = sizeof(ch->cl_addr);
    sockfd = (unsigned long) ch->socket;
    memset(buffer, 0, BUF_SIZE);  
    while (fgets(buffer, BUF_SIZE, stdin) != NULL) {
        ret = ::sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &ch->cl_addr, len);  
        if (ret < 0) {  
            printf("Error sending data!\n");  
            exit(1);
        }
    }   
}

int test() {

    sox_api srv;

    unsigned long ret, newsockfd;
    socklen_t len;
    char buffer[BUF_SIZE];
    char clientAddr[CLADDR_LEN];
    pthread_t rThread, sThread;
    memset(&srv.addr, 0, sizeof(srv.addr));
    srv.addr.sin_family = AF_INET;
    srv.addr.sin_addr.s_addr = INADDR_ANY;
    srv.addr.sin_port = PORT; 
    ret = ::bind(srv.sockfd, (struct sockaddr *) &srv.addr, sizeof(srv.addr));
    if (ret < 0) {
        printf("Error binding!\n");
        exit(1);
    }
    printf("Binding done...\n");
listener:
    printf("Waiting for a connection...\n");
    listen(srv.sockfd, 5);
    len = sizeof(srv.cl_addr);
    newsockfd = ::accept(srv.sockfd, (struct sockaddr *) &srv.cl_addr, &len);
    if (newsockfd < 0) {
        printf("Error accepting connection!\n");
        exit(1);
    } 
    inet_ntop(AF_INET, &(srv.cl_addr.sin_addr), clientAddr, CLADDR_LEN);
    printf("Connection accepted from %s...\n", clientAddr); 

    memset(buffer, 0, BUF_SIZE);
    printf("Enter your messages one by one and press return key!\n");
    //creating a new thread for receiving messages from the client
    ret = pthread_create(&rThread, NULL, receiveMessage, (void *) newsockfd);
    if (ret) {
        printf("ERROR: Return Code from pthread_create() is %d\n", ret);
        exit(1);
    }
    struct chat_t *ch = (struct chat_t *) malloc(sizeof(struct chat_t));
    ch->socket = (void *) newsockfd;
    ch->cl_addr = srv.cl_addr;
    //creating a new thread for sending messages to the client
    ret = pthread_create(&sThread, NULL, sendMessage, (void *) ch);
    if (ret) {
        printf("ERROR: Return Code from pthread_create() is %d\n", ret);
        exit(1);
    }
    goto listener;
    while (1) ;
    close(newsockfd);
    pthread_exit(NULL);
    return 0;
}

int main()
{
    test();
}
