// Server program
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "func_Server.h"
#include "func_Server.h"
#define PORT_DEFAULT 58033
#define MAXLINE 1024

int max(int x, int y){
    if (x > y)
        return x;
    else
        return y;
}

int main(){

    int listenfd, connfd, udpfd,nready, maxfdp1;
    char buffer[MAX_OUT_SIZE];
    fd_set rset;
    
    socklen_t len;
    struct sockaddr_in cliaddr, servaddr;
    char *out;
    void sig_chld(int);
 
    /* create listening TCP socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT_DEFAULT);
 
    // binding server addr structure to listenfd
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(listenfd, 10);
 
    /* create UDP socket */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    // binding server addr structure to udp sockfd
    bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
 
    // clear the descriptor set
    FD_ZERO(&rset);
 
    // get maxfd
    maxfdp1 = max(listenfd, udpfd) + 1;
    for (;;) {
 
        // set listenfd and udpfd in readset
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
 
        // select the ready descriptor
        nready = select(maxfdp1, &rset, NULL, NULL, NULL);
        if(nready){
        // if tcp socket is readable then handle
        // it by accepting the connection
        if (FD_ISSET(listenfd, &rset)) {
            len = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
            close(listenfd);
            bzero(buffer, sizeof(buffer));
            printf("Message From TCP client: ");
            if(read(connfd, buffer, sizeof(buffer))){
            puts(buffer);
            if(!write(connfd, (const char*)buffer, sizeof(buffer))){
                close(connfd);
                continue;
                }
            }
            else
                continue;
        }
        // if udp socket is readable receive the message.
        if (FD_ISSET(udpfd, &rset)) {
            len = sizeof(cliaddr);
            bzero(buffer, sizeof(buffer));
            if(recvfrom(udpfd, buffer, sizeof(buffer), 0,(struct sockaddr*)&cliaddr, &len)){
            out = processInput(buffer);
            sendto(udpfd, (const char*)out, strlen(out), 0,(struct sockaddr*)&cliaddr, sizeof(cliaddr));
            }
            else
                continue;
        }
        }
    }
    return 0;
}
