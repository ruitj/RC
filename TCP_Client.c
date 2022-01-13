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
#include "TCP_Client.h"

int fd_tcp, errcode_tcp;
ssize_t n_tcp;
struct addrinfo hints_tcp, *res_tcp;

int TimerON_TCP(int sd){
    struct timeval tmout;
    
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    tmout.tv_sec=5; /* Wait for 5 sec for a reply from server. */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}
    
int TimerOFF_TCP(int sd){
    struct timeval tmout;
    
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}

void initTCP(char hostName[], char port[]){
    fd_tcp=socket(AF_INET, SOCK_STREAM, 0);
    if (fd_tcp==-1) exit(1);

    memset(&hints_tcp, 0, sizeof hints_tcp);
    hints_tcp.ai_family = AF_INET;
    hints_tcp.ai_socktype = SOCK_STREAM;
    errcode_tcp=getaddrinfo(hostName, port, &hints_tcp, &res_tcp);
    if (errcode_tcp!=0) exit(1);

}

void connectTCP(){
    n_tcp=connect(fd_tcp, res_tcp->ai_addr, res_tcp->ai_addrlen);
    if(n_tcp==-1){
        fprintf(stderr, "Error: could not connect to server\n");
        exit(1);
    }
}

int writeTCP(char *msg, int n_bytes){
    n_tcp=write(fd_tcp, msg, n_bytes);
    if(n_tcp==-1) exit(1);
    return n_tcp;
}

int readTCP(int n_bytes, char *content){
    n_tcp=read(fd_tcp, content, n_bytes);
    if(n_tcp==-1) exit(1);
    content[n_tcp] = '\0';
    return n_tcp;
}

void closeTCP(){
    close(fd_tcp);
    freeaddrinfo(res_tcp);
}
