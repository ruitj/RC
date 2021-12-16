#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "Client.h"
#include "func_Client.h"
#include "TCP_Client.h"
#include <signal.h>

char port[MAX_PORT_SIZE], *hostName;

void processInput(){
    char input[MAX_INPUT_SIZE];

    while (fgets(input, sizeof(input)/sizeof(char), stdin)){
        char optype[MAX_OPTYPE_SIZE];
        int i = 0;
        while ((input[i] != ' ') && (input[i] != '\n')){
            optype[i] = input[i];
            i++;
        }
        optype[i] = '\0';
        if (strcmp(optype, "reg") == 0){
            registerUser(&input[i+1]);
        }
        else if ((strcmp(optype, "unr") == 0) || (strcmp(optype, "unregister") == 0)){
            unregisterUser(&input[i+1]);
        }
        else if (strcmp(optype, "login") == 0){
            
            loginUser(&input[i+1]);
        }
        else if (strcmp(optype, "logout") == 0){
            logoutUser(&input[i+1]);
        }
        else if (strcmp(optype, "exit") == 0){
            exitSession();
            break;
        }
        else if ((strcmp(optype, "groups") == 0) || (strcmp(optype, "gl") == 0)){
            showAvailableGroups();
        }
        else if ((strcmp(optype, "subscribe") == 0) || (strcmp(optype, "s") == 0)){
            subscribeGroup(&input[i+1]);
        }
        else if ((strcmp(optype, "unsubscribe") == 0) || (strcmp(optype, "u") == 0)){
            unsubscribeGroup(&input[i+1]);
        }
        else if ((strcmp(optype, "my_groups") == 0) || (strcmp(optype, "mgl") == 0)){
            showMyGroups();
        }
        else if ((strcmp(optype, "select") == 0) || (strcmp(optype, "sag") == 0)){
            selectGroup(&input[i+1]);
        }
        else if ( strcmp(optype,"ulist")==0 || strcmp(optype,"ul")==0 ){
           listUsers_GID();
           closeTCP();
           initTCP("tejo.tecnico.ulisboa.pt","58011");
        }
        else{ /* default case */
            printf("Error: invalid operation: %s\n", optype);
        }
    }
}

/*int parseArgs(int n, char **args){
    if(n > 1){
        if(strcmp(args[1],"-n") == 0){
            IP = args[2];
            if((n == 5) && (strcmp(args[3],"-p") == 0)){
                port = args[4];
            }
            else{
                fprintf(stderr,"Error: invalid arguments\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(args[1],"-p") == 0){
            port = args[2];
        }
        else{
            fprintf(stderr,"Error: invalid arguments\n");
        }
    }
}*/

int main(int argc, char**argv){

    //input parsing
    //parseArgs(argc, argv)
    //struct hostent *host_entry;
    //char hostbuffer[256];
    //gethostname(hostbuffer, sizeof(hostbuffer));
    //printf("%s\n", hostbuffer);
       struct sigaction act;
    strcpy(port,"58011");

    memset(&act,0, sizeof act);
   
   //act.sa_handler=SIG_IGN;
    signal(SIGPIPE, SIG_IGN);


    initSession("tejo.tecnico.ulisboa.pt", port);
    processInput();
    

    return 0;
}

//gcc -O3 -Wall UDP_Client.c TCP_Client.c func_Client.c user.c -lm -o user