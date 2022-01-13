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
#define PORT_DEFAULT "58033"
#define MAXLINE 1024

char port[10];
int verbose_mode = 0;

int max(int x, int y){
    if (x > y)
        return x;
    else
        return y;
}

int TimerON_S(int sd){
    struct timeval tmout;
    
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    tmout.tv_sec=5; /* Wait for 5 sec for a reply from server. */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}
    
int TimerOFF_S(int sd){
    struct timeval tmout;
    
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}

int receivecmd(){
    int listenfd, connfd, udpfd,nready, maxfdp1;
    char buffer[MAX_OUT_SIZE];
    fd_set rset;
    
    socklen_t len;
    struct sockaddr_in cliaddr, servaddr;
    char *out,input[5];
 
    /* create listening TCP socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(port));
 
    int n;
    // binding server addr structure to listenfd
    n = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (n != 0){
        fprintf(stderr, "Error: bind failure\n");
        exit(EXIT_FAILURE);
    }
    n = listen(listenfd, 10);
    if (n != 0){
        fprintf(stderr, "Error: listen failure\n");
        exit(EXIT_FAILURE);
    }
 
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
                if (connfd != -1){
                    bzero(buffer, sizeof(buffer));
                    /*if (TimerON_S(connfd) < 0)
                        continue;*/
                    if(read(connfd,input,4)>0){
                        /*if (TimerOFF_S(connfd) < 0)
                            continue;*/
                        if (verbose_mode)
                            printf("Request from IP: %s; port: %d\n", inet_ntoa(cliaddr.sin_addr), (int) ntohs(cliaddr.sin_port));
                        processInputTCP(connfd,input);
                    }
                }
                close(connfd);
            }
            // if udp socket is readable receive the message.
            if (FD_ISSET(udpfd, &rset)) {
                len = sizeof(cliaddr);
                bzero(buffer, sizeof(buffer));
                /*if (TimerON_S(udpfd) < 0)
                    exit(1);*/
                if(recvfrom(udpfd, buffer, sizeof(buffer), 0,(struct sockaddr*)&cliaddr, &len)){
                    /*if (TimerOFF_S(udpfd) < 0)
                        continue;*/
                    if (verbose_mode)
                        printf("Request from IP: %s; port: %d\n", inet_ntoa(cliaddr.sin_addr), (int) ntohs(cliaddr.sin_port));
                    out = processInput(buffer);
                    sendto(udpfd, (const char*)out, strlen(out), 0,(struct sockaddr*)&cliaddr, sizeof(cliaddr));
                }
            }
        }
    }
}

int parseArgs(int n, char **args){
    if (n > 1){
        if (n >= 3){
            if (strcmp(args[1], "-p") == 0){
                sprintf(port, "%s", args[2]);
                if (n == 4){
                    if (strcmp(args[3], "-v") == 0)
                        verbose_mode = 1;
                }
                else if (n != 3){
                    return 0;
                }
            }
            else if (strcmp(args[1], "-v") == 0){
                verbose_mode = 1;
            }
            else{
                return 0;
            }
        }
        else if (strcmp(args[1], "-v") == 0){
            sprintf(port, "%s", PORT_DEFAULT);
            verbose_mode = 1;
        }
        else{
            return 0;
        }
    }
    else{
        sprintf(port, "%s", PORT_DEFAULT);
    }
    return 1;
}

int main(int argc, char **argv){

    //input parsing
    int p = parseArgs(argc, argv);
    if (p){
        initSession(verbose_mode);
        receivecmd();   
    }
    else{
        printf("Error: invalid input arguments\n");
    }
    return 0;
}
