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
#include "UDP_Client.h"

int fd_udp, errcde_udp;
ssize_t n_udp;
socklen_t addrlen_udp;
struct addrinfo hints_udp, *res_udp;
struct sockaddr_in addr_udp;
char buffer_udp[MAX_OUTUDP_SIZE];

int TimerON_UDP(int sd){
    struct timeval tmout;
    
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    tmout.tv_sec=5; /* Wait for 5 sec for a reply from server. */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}
    
int TimerOFF_UDP(int sd){
    struct timeval tmout;
    
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}

void initUDP(char hostName[], char port[]){
    fd_udp=socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_udp==-1) exit(1);

    memset(&hints_udp, 0, sizeof hints_udp);
    hints_udp.ai_family = AF_INET;
    hints_udp.ai_socktype = SOCK_DGRAM;

    errcde_udp=getaddrinfo(hostName, port, &hints_udp, &res_udp);
    if (errcde_udp!=0) exit(1);

}

char *sendUDP(char *msg){
    n_udp=sendto(fd_udp, msg, strlen(msg), 0, res_udp->ai_addr, res_udp->ai_addrlen);
    if (n_udp==-1) exit(1);
    addrlen_udp=sizeof(addr_udp);
    
    n_udp=recvfrom(fd_udp, (char *)buffer_udp, MAX_OUTUDP_SIZE-1, 0, (struct sockaddr*)&addr_udp, &addrlen_udp);
    
    if (n_udp==-1) exit(1);
    buffer_udp[n_udp] = '\0';
    return buffer_udp;
}

void closeUDP(){
    freeaddrinfo(res_udp);
    close(fd_udp);
}

