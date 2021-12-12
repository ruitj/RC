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
    for (i = 0; input[i] != '\n';i++){
        if (!(((isdigit(input[i])) || (isalpha(input[i]))) || (i < 8)))
            return 0;
    }
    return 1;
}

int registerUser(char *input){
    char in[MAX_INPUT_SIZE], *out;
    //int i=0;
    //int spacesCount=0, spaceIndex=0;
    //int status;
    //verificacao do input (ver se tem 1 espaco, exatamente 14 carateres, UID so integers e 5 carateres e password com 8 carateres, letras e numeros)
    /*for(i;input[i]!='\n';i++){
        if (input[i] == ' '){
            espacosCount++;
            espacoIndex = i;
        } 
    }
    if(i!=14 || espacoIndex!=5){
        //print error
        fprintf(stderr,"Error: wrong input format\n");
        return 0;
    }
    if (espacosCount == 1){
        for(i=0;i!=espacoIndex;i++){
            if(isdigit(input[i])==0){
                //print error
                fprintf(stderr,"Error: wrong input format\n");
                return 0;
            }
        }
        for(i=espacoIndex+1;i<14;i++){
            if(isalpha(input[i])==0 && isdigit(input[i])==0){
                //print error
                fprintf(stderr,"Error: wrong input format\n");
                return 0;
            }
        }
    }
    else{
        printf("Error: wrong input format\n");
        return 0;
        //print erro
    }*/

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
