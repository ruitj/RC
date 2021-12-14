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

int fd, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[MAX_OUTPUT_SIZE];
char HOST[256], PORT[10]; 

void initTCP(char hostName[], char port[]){
    fd=socket(AF_INET, SOCK_STREAM, 0);
    if (fd==-1) exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    strcpy(HOST, hostName);
    strcpy(PORT, port);
    
    errcode=getaddrinfo(HOST, PORT, &hints, &res);
    if (errcode!=0) exit(1);

}

/**
 * duvida no funcionamento do read e no caso especifico de
 * termos uma string com um multiplo do tamanho do buffer;
 * return da funcao read  qd nao le nada ou le 'EOF'/'\0';
 * read/recvfrom colocam '\0' no buffer passado como arg??
 */
char *sendTCP(char *msg){
    n=connect(fd, res->ai_addr, res->ai_addrlen);
    if(n==-1) exit(1);
    n=write(fd, msg, strlen(msg));
    if(n==-1) exit(1);

    while (1){
        n=read(fd, buffer, MAX_OUTPUT_SIZE-1);
        if(n==-1)
            exit(1);
        buffer[n] = '\0';
        printf("%s", buffer);
        if(strlen(buffer) != MAX_OUTPUT_SIZE-1)
            break;
    }
    close(fd);
    return buffer;
}

void closeTCP(){
    freeaddrinfo(res);
    close(fd);
}

