#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include "Client.h"

int fd_tcp, errcode_tcp;
ssize_t n_tcp;
struct addrinfo hints_tcp, *res_tcp;
char buffer_tcp[MAX_OUTPUT_SIZE];
//char saved_buffer[MAX_OUTPUT_SIZE]; 

void initTCP(char hostName[], char port[]){
    fd_tcp=socket(AF_INET, SOCK_STREAM, 0);
   // printf("%d\n",fd_tcp);
    if (fd_tcp==-1) exit(1);

    memset(&hints_tcp, 0, sizeof hints_tcp);
    hints_tcp.ai_family = AF_INET;
    hints_tcp.ai_socktype = SOCK_STREAM;
    errcode_tcp=getaddrinfo(hostName, port, &hints_tcp, &res_tcp);
    if (errcode_tcp!=0) exit(1);

}

char *sendTCP(char *msg, int max){
    n_tcp=connect(fd_tcp, res_tcp->ai_addr, res_tcp->ai_addrlen);
    if(n_tcp==-1) exit(1);
    n_tcp=write(fd_tcp,msg, strlen(msg));
    if(n_tcp==-1) exit(1);

    n_tcp=read(fd_tcp, buffer_tcp, max);
    return buffer_tcp;
}

char *readTCP(){
    n_tcp=read(fd_tcp, buffer_tcp, MAX_OUTPUT_SIZE-1);
    if(n_tcp==-1)
        exit(1);
    buffer_tcp[n_tcp] = '\0';
    return buffer_tcp;
}

void closeTCP(){
    freeaddrinfo(res_tcp);
    close(fd_tcp);
}

