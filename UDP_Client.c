#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <Client.h>

int fd, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[128];

void initUDP(char IP[], char port[]){
    fd=socket(AF_INET, SOCK_DGRAM, 0);
    if (FD==-1) exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errcode=getaddrinfo(IP, port, &hints, &res);
    if (errcode!=0) exit(1);
}
char *sendUDP(char *msg){
    n=sendto(fd, msg, len(msg), 0, res->ai_addr, res->ai_addrlen);
    if (n==-1) /*error*/ exit(1);
    addrlen=sizeof(addr);
    n=recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
    if (n==-1) /*error*/ exit(1);
    return buffer;
}

void closeUDP(){
    freeaddrinfo(res);
    close(fd);
}
