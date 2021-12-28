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

char port[MAX_PORT_SIZE], hostName[MAX_HOST_SIZE];

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
            if (input[i] == ' '){
                registerUser(&input[i+1]);
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if ((strcmp(optype, "unr") == 0) || (strcmp(optype, "unregister") == 0)){
            if (input[i] == ' '){
                unregisterUser(&input[i+1]);
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if (strcmp(optype, "login") == 0){
            if (input[i] == ' '){
                loginUser(&input[i+1]);
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if (strcmp(optype, "logout") == 0){
            if (strcmp(&input[i], "\n") == 0){
                logoutUser();
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if (strcmp(optype, "exit") == 0){
            if (strcmp(&input[i], "\n") == 0){
                exitSession();
                break;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if ((strcmp(optype, "groups") == 0) || (strcmp(optype, "gl") == 0)){
            if (strcmp(&input[i], "\n") == 0){
                showAvailableGroups();
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if ((strcmp(optype, "subscribe") == 0) || (strcmp(optype, "s") == 0)){
            if (input[i] == ' '){
                subscribeGroup(&input[i+1]);
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if ((strcmp(optype, "unsubscribe") == 0) || (strcmp(optype, "u") == 0)){
            if (input[i] == ' '){
                unsubscribeGroup(&input[i+1]);
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if ((strcmp(optype, "my_groups") == 0) || (strcmp(optype, "mgl") == 0)){
            if (strcmp(&input[i], "\n") == 0){
                showMyGroups();
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if ((strcmp(optype, "select") == 0) || (strcmp(optype, "sag") == 0)){
            if (input[i] == ' '){
                selectGroup(&input[i+1]);
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if ((strcmp(optype, "showgid") == 0) || (strcmp(optype, "sg") == 0)){
            if (strcmp(&input[i], "\n") == 0){
                showGIDSelected();
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if ((strcmp(optype,"ulist") == 0) || (strcmp(optype,"ul") == 0)){
            if (strcmp(&input[i], "\n") == 0){
                initTCP(hostName, port);
                listUsers_GID();
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if (strcmp(optype, "post") == 0){
            if (input[i] == ' '){
                initTCP(hostName, port);
                postMessage(&input[i+1]);
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else if ((strcmp(optype, "retrieve") == 0) || (strcmp(optype, "r") == 0)){
            if (input[i] == ' '){
                initTCP(hostName, port);
                retrieveMessages(&input[i+1]);
                continue;
            }
            printf("Error: invalid operation: %s", input);
        }
        else{ /* default case */
            printf("Error: invalid operation: %s\n", optype);
        }
    }
}

int parseArgs(int n, char **args){
    if(n > 1){
        if (n >= 3){
            if(strcmp(args[1],"-n") == 0){
                strcpy(hostName, args[2]);
                if((n == 5) && (strcmp(args[3],"-p") == 0)){
                    strcpy(port, args[4]);
                    return 1;
                }
                else if (n == 3){
                    strcpy(port, PORT_DEFAULT);
                    return 1;
                }
                else{
                    fprintf(stderr,"Error: invalid arguments\n");
                    exit(1);
                }
            }
            else if(strcmp(args[1],"-p") == 0){
                strcpy(port, args[2]);
                gethostname(hostName, sizeof(hostName));
                return 1;
            }
            else{
                fprintf(stderr,"Error: invalid arguments\n");
                exit(1);
            }
        }
    }
    return 0;
}

int main(int argc, char**argv){

    //input parsing
    int p = parseArgs(argc, argv);
    if (p == 0){
        gethostname(hostName, sizeof(hostName));
        strcpy(port, PORT_DEFAULT);
    }

    initSession(hostName, port);
    processInput();
    
    return 0;
}

//gcc -O3 -Wall UDP_Client.c TCP_Client.c func_Client.c user.c -lm -o user
