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

int fd_tcp, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[MAX_OUTPUT_SIZE];
char HOST[256], PORT[10]; 
char saved_buffer[MAX_OUTPUT_SIZE];

void initTCP(char hostName[], char port[]){
    signal(SIGPIPE,SIG_IGN);
    fd_tcp=socket(AF_INET, SOCK_STREAM, 0);
   // printf("%d\n",fd_tcp);
    if (fd_tcp==-1) exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    strcpy(HOST, hostName);
    strcpy(PORT, port);
    errcode=getaddrinfo("tejo.tecnico.ulisboa.pt", PORT_DEFAULT, &hints, &res);
    if (errcode!=0) exit(1);

}

/**
 * duvida no funcionamento do read e no caso especifico de
 * termos uma string com um multiplo do tamanho do buffer;
 * return da funcao read  qd nao le nada ou le 'EOF'/'\0';
 * read/recvfrom colocam '\0' no buffer passado como arg??
 */
char *sendTCP(char *msg){
    signal(SIGPIPE,SIG_IGN);
    n=connect(fd_tcp, res->ai_addr, res->ai_addrlen);
   // puts("response");
    if(n==-1) exit(1);
    int buffer_number=0,ptr=0;
    //puts("response");
    n=write(fd_tcp,msg, strlen(msg));
    if(n==-1) exit(1);

    while (1){
        n=read(fd_tcp, buffer, MAX_OUTPUT_SIZE-1);
        if(n==-1)
            exit(1);
        buffer[n] = '\0';
        buffer_number++;
        //printf("%ld\n",strlen(buffer));
        if(strlen(buffer) != MAX_OUTPUT_SIZE-1 && strlen(buffer)==0 ){
            saved_buffer[ptr]='\0';
          break;
        }
        else{
            //printf("buffer_number=%d\n",buffer_number);
            //printf("%s",buffer);
            for(int i=0;i<strlen(buffer);i++){
                saved_buffer[ptr]=buffer[i];
                ptr+=1;
            }
            saved_buffer[ptr]=' ';
            
        }
    }
    //printf("ptr=%d\n",ptr);
    //printf("%s",saved_buffer);
    close(fd_tcp);
    
    return saved_buffer;
}

void closeTCP(){
    freeaddrinfo(res);
    close(fd_tcp);
}

