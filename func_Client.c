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
        return =;
    for (i = 0; input[i] != '\n';i++){
        if (!(((isdigit(input[i])) || (isalpha(input[i]))) || (i < 8)))
            return 0;
    }
    return 1;
}

int registerUser(char *input){
    char in[MAX_INPUT_SIZE], *out;
    
    if (!validUID(input)){
        printf("Warning: Invalid UID\n");
        return 0;
    }
    if (!((input[5] == ' ') && (input[6] != ' '))){
        printf("Warning: Invalid input format\n");
        return 0;
    }
    if (!validPassword(&input[6])){
        printf("Warning: Invalid password\n");
        return 0;
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
    return 0;
}
