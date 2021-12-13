#include "Client.h"
#include "UDP_Client.h"
#include "TCP_Client.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

char savedUID[MAX_UID_SIZE], savedPass[MAX_PASS_SIZE];
int loggedin = 0;

int validUID(char *input){
    for (int i = 0; input[i] != ' ';i++){
        if (!((isdigit(input[i])) || (i < 5)))
            return 0;
    }
    return 1;
}

int validPassword(char *input){
    if (strlen(input) != 9)
        return 0;
    for (int i = 0; input[i] != '\n';i++){
        if (!(((isdigit(input[i])) || (isalpha(input[i]))) || (i < 8)))
            return 0;
    }
    return 1;
}

void registerUser(char *input){
    char in[MAX_INPUT_SIZE], *out;

    if (!validUID(input)){
        printf("Error: Invalid input format\n");
        return;
    }
    if (!((input[5] == ' ') && (input[6] != ' '))){
        printf("Error: Invalid input format\n");
        return;
    }
    if (!validPassword(&input[6])){
        printf("Error: Invalid input format\n");
        return;
    }

    sprintf(in, "REG %s", input);
    out = sendUDP(in);

    if (strcmp(out, "RRG OK\n") == 0){
        printf("User successfully registered\n");
    }
    else if (strcmp(out, "RRG NOK\n") == 0){
        printf("Invalid credentials\n");
    }
    else if (strcmp(out, "RRG DUP\n") == 0){
        printf("User already registered\n");
    }
    else{
        printf("Warning: wrong message format\n");
    }
}

void unregisterUser(char *input){
    char in[MAX_INPUT_SIZE], *out;

    if (!validUID(input)){
        printf("Error: Invalid input format\n");
        return;
    }
    if (!((input[5] == ' ') && (input[6] != ' '))){
        printf("Error: Invalid input format\n");
        return;
    }
    if (!validPassword(&input[6])){
        printf("Error: Invalid input format\n");
        return;
    }

    sprintf(in, "UNR %s", input);
    out = sendUDP(in);

    if (strcmp(out, "RUN OK\n") == 0){
        printf("User successfully unregistered\n");
    }
    else if (strcmp(out, "RUN NOK\n") == 0){
        printf("Invalid credentials\n");
    }
    else{
        printf("Warning: wrong message format\n");
    }
}

void loginUser(char *input){
    char in[MAX_INPUT_SIZE], *out;

    if (loggedin){
        printf("Error: User already logged in\n");
        return;
    }

    if (!validUID(input)){
        printf("Error: Invalid input format\n");
        return;
    }
    if (!((input[5] == ' ') && (input[6] != ' '))){
        printf("Error: Invalid input format\n");
        return;
    }
    if (!validPassword(&input[6])){
        printf("Error: Invalid input format\n");
        return;
    }

    sprintf(in, "LOG %s", input);
    out = sendUDP(in);

    if (strcmp(out, "RLO OK\n") == 0){
        printf("You are now logged in\n");
    }
    else if (strcmp(out, "RLO NOK\n") == 0){
        printf("Invalid credentials\n");
    }
    else{
        printf("Warning: wrong message format\n");
    }

    // save credentials (UID and Password)
    int i, j;
    for (i = 0, j = 0; i < 5; i++){
        savedUID[j] = input[i];
        j++;
    }
    savedUID[j] = '\0';
    
    for (i += 1, j = 0; i < 14; i++){
        savedPass[j] = input[i];
        j++;
    }
    savedPass[j] = '\0';
    loggedin = 1;

}

void logoutUser(){
    char in[MAX_INPUT_SIZE], *out;
    
    if (!loggedin){
        printf("Error: User not logged in\n");
        return;
    }

    sprintf(in, "OUT %s %s\n", savedUID, savedPass);
    out = sendUDP(in);

    if (strcmp(out, "ROU OK\n") == 0){
        printf("You are now logged out\n");
    }
    else if (strcmp(out, "ROU NOK\n") == 0){
        printf("Invalid credentials\n");
    }
    else{
        printf("Warning: wrong message format\n");
    }

    // forget credentials (UID and Password)
    savedUID[0] = '\0';
    savedPass[0] = '\0';
    loggedin = 0;

}

void showAvailableGroups(){
    char in[MAX_INPUT_SIZE], *out;

    sprintf(in, "GLS\n");
    out = sendUDP(in);

    printf("%s", out);
    
}

void exitSession(){
    closeUDP();
}
