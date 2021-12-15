#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include "Client.h"

int fd_udp, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[MAX_OUTPUT_SIZE];
char HOST[256], PORT[10]; 

void initUDP(char hostName[], char port[]){
    fd_udp=socket(AF_INET, SOCK_DGRAM, 0);
    //printf("%d\n",fd_udp);
    if (fd_udp==-1) exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    strcpy(HOST, hostName);
    strcpy(PORT, port);
    
    errcode=getaddrinfo(HOST, PORT, &hints, &res);
    if (errcode!=0) exit(1);

}

char *sendUDP(char *msg){
    n=sendto(fd_udp, msg, strlen(msg), 0, res->ai_addr, res->ai_addrlen);
    if (n==-1){ 
       exit(1);
    }
    addrlen=sizeof(addr);
    
    n=recvfrom(fd_udp, (char *)buffer, MAX_OUTPUT_SIZE-1, 0, (struct sockaddr*)&addr, &addrlen);
    if (n==-1){  
        exit(1);
    }
    buffer[n] = '\0';
    return buffer;
}

void closeUDP(){
    freeaddrinfo(res);
    close(fd_udp);
}
