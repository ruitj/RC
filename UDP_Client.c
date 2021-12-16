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

int fd_udp, errcde_udp;
ssize_t n;
socklen_t addrlen_udp;
struct addrinfo hints_udp, *res_udp;
struct sockaddr_in addr_udp;
char buffer_udp[MAX_OUTPUT_SIZE];

void initUDP(char hostName[], char port[]){
    fd_udp=socket(AF_INET, SOCK_DGRAM, 0);
    //printf("%d\n",fd_udp);
    if (fd_udp==-1) exit(1);

    memset(&hints_udp, 0, sizeof hints_udp);
    hints_udp.ai_family = AF_INET;
    hints_udp.ai_socktype = SOCK_DGRAM;

    errcde_udp=getaddrinfo(hostName, port, &hints_udp, &res_udp);
    if (errcde_udp!=0) exit(1);

}

char *sendUDP(char *msg){
    n=sendto(fd_udp, msg, strlen(msg), 0, res_udp->ai_addr, res_udp->ai_addrlen);
    if (n==-1){ 
       exit(1);
    }
    addrlen_udp=sizeof(addr_udp);
    
    n=recvfrom(fd_udp, (char *)buffer_udp, MAX_OUTPUT_SIZE-1, 0, (struct sockaddr*)&addr_udp, &addrlen_udp);
    
    if (n==-1){  
        exit(1);
    }
    buffer_udp[n] = '\0';
    return buffer_udp;
}

void closeUDP(){
    freeaddrinfo(res_udp);
    close(fd_udp);
}
