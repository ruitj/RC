#include "Client.h"
#include "UDP_Client.h"
#include "TCP_Client.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

int validUID(char *input){
    int i;
    for (i = 0; input[i] != ' ';i++){
        if (!((isdigit(input[i])) || (i < 5)))
            return 0;
    }
    return 1;
}

int validPassword(char *input){
    int i;
    if (strlen(input) != 9)
        return 0;
    for (i = 0; input[i] != '\n';i++){
        if (!(((isdigit(input[i])) || (isalpha(input[i]))) || (i < 8)))
            return 0;
    }
    return 1;
}

void registerUser(char *input){
    char in[MAX_INPUT_SIZE], *out;

    if (!validUID(input)){
        printf("Warning: Invalid UID\n");
        return;
    }
    if (!((input[5] == ' ') && (input[6] != ' '))){
        printf("Warning: Invalid input format\n");
        return;
    }
    if (!validPassword(&input[6])){
        printf("Warning: Invalid password\n");
        return;
    }

    sprintf(in, "REG %s", input);
    out = sendUDP(in);

    if (strcmp(out, "RRG OK\n") == 0){
        printf("User successfully registered\n");
    }
    else if (strcmp(out, "RRG NOK\n") == 0){
        printf("Warning: User not registered\n");
    }
    else if (strcmp(out, "RRG DUP\n") == 0){
        printf("Warning: User already registered\n");
    }
    else{
        printf("Warning: wrong message format\n");
    }
}

void unregisterUser(char *input){
    char in[MAX_INPUT_SIZE], *out;

    if (!validUID(input)){
        printf("Warning: Invalid UID\n");
        return;
    }
    if (!((input[5] == ' ') && (input[6] != ' '))){
        printf("Warning: Invalid input format\n");
        return;
    }
    if (!validPassword(&input[6])){
        printf("Warning: Invalid password\n");
        return;
    }

    sprintf(in, "UNR %s", input);
    out = sendUDP(in);

    if (strcmp(out, "RUN OK\n") == 0){
        printf("User successfully unregistered\n");
    }
    else if (strcmp(out, "RUN NOK\n") == 0){
        printf("Warning: User does not exist\n");
    }
    else{
        printf("Warning: wrong message format\n");
    }
}

void exitSession(){
    closeUDP();
}
