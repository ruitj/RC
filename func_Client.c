#include "Client.h"
#include "UDP_Client.h"
#include "TCP_Client.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char savedUID[MAX_UID_SIZE], savedPass[MAX_PASS_SIZE];
int loggedin = 0;

int validUID(char *input){
    for (int i = 0; ((input[i] != ' ') && (input[i] != '\n'));i++){
        if ((!isdigit(input[i])) || (i >= 5))
            return 0;
    }
    return 1;
}

int validPassword(char *input){
    if (strlen(input) != 9)
        return 0;
    for (int i = 0; input[i] != '\n';i++){
        if (((!isdigit(input[i])) && (!isalpha(input[i]))) || (i >= 8))
            return 0;
    }
    return 1;
}

int validGID(char *input){
    for (int i = 0; ((input[i] != ' ') && (input[i] != '\n')); i++){
        if ((!isdigit(input[i])) || (i >= 2))
            return 0;
    }
    return 1;
}

int validGName(char *input){
    if (strlen(input) >= 26)
        return 0;
    for (int i = 0; input[i] != '\n';i++){
        if (((!isdigit(input[i])) && (!isalpha(input[i])) && (input[i] != '-') && (input[i] != '_')) || (i >= 24))
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
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: Invalid message format\n");
        return;
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
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: Invalid message format\n");
        return;
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
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: Invalid message format\n");
        return;
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
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: Invalid message format\n");
        return;
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

    if (strcmp(out, "RGL 0\n") == 0){
        printf("No groups available\n");
        return;
    }
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: Invalid message format\n");
        return;
    }

    char Groups[3];
    int i = 4;
    int j = 0;
    while (out[i] != ' '){
        Groups[j] = out[i];
        i++; j++;
    }
    Groups[j] = '\0';

    printf("Available groups:\n");

    int nGroups = atoi(Groups);
    char GID[MAX_GID_SIZE], GName[MAX_GNAME_SIZE], MID[MAX_MID_SIZE];
    i += 1;
    for (int n = 0; n < nGroups; n++){
        sscanf(&out[i], "%s %s %s", GID, GName, MID);
        i += strlen(GID) + strlen(GName) + strlen(MID) + 3;
        printf("Group ID: %s\t\tGroup Name: %-24s\t\tLatest MID: %s\n", GID, GName, MID);
    }

}

void subscribeGroup(char *input){
    char in[MAX_INPUT_SIZE], *out;

    if (!loggedin){
        printf("Error: User not logged in\n");
        return;
    }

    if (!validGID(input)){
        printf("Error: Invalid input format\n");
        return;
    }

    int i;
    for (i = 0; input[i] != ' '; i++){}

    if (!((input[i] == ' ') && (input[i+1] != ' '))){
        printf("Error: Invalid input format\n");
        return;
    }
    if (!validGName(&input[i+1])){
        printf("Error: Invalid input format\n");
        return;
    }

    char GID[MAX_GID_SIZE], GName[MAX_GNAME_SIZE];
    sscanf(input, "%s %s", GID, GName);

    if (input[0] == '0'){
        strcpy(GID, "00");
        GID[2] = '\0';
    }

    sprintf(in, "GSR %s %s %s\n", savedUID, GID, GName);
    out = sendUDP(in);
    
    char out_msg[12];
    int j;

    for (j = 0; (!isdigit(out[j]) && (out[j] != '\n')); j++){
        out_msg[j] = out[j];
    }
    
    if (out_msg[j-1] == ' ')
        out_msg[j-1] = '\0';

    if (strcmp(out_msg, "RGS NEW") == 0){
        sscanf(&out[j], "%s", GID);
        printf("New group created and subscribed: %s %s\n", GID, GName);
    }
    else if (strcmp(out, "RGS OK\n") == 0){
        sscanf(&out[j], "%s", GID);
        printf("New group subscribed: %s %s\n", GID, GName);
    }
    else if (strcmp(out, "RGS NOK\n") == 0){
        printf("Invalid credentials\n");
    }
    else if (strcmp(out, "RGS E_USR\n") == 0){
        printf("Invalid user ID\n");
    }
    else if (strcmp(out, "RGS E_GRP\n") == 0){
        printf("Invalid group ID\n");
    }
    else if (strcmp(out, "RGS E_GNAME\n") == 0){
        printf("Invalid group name\n");
    }
    else if (strcmp(out, "RGS E_FULL\n") == 0){
        printf("Maximum number of groups achieved\n");
    }
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: Invalid message format\n");
        return;
    }

}

void unsubscribeGroup(char *input){
    char in[MAX_INPUT_SIZE], *out;
    
    if (!loggedin){
        printf("Error: User not logged in\n");
        return;
    }

    if (!validGID(input)){
        printf("Error: Invalid input format\n");
        return;
    }

    char GID[MAX_GID_SIZE];
    sscanf(input, "%s", GID);
    sprintf(in, "GUR %s %s\n", savedUID, GID);
    out = sendUDP(in);

    if (strcmp(out, "RGU OK\n") == 0){
        printf("Unsubscribed from group %s successfully\n", GID);
    }
    else if (strcmp(out, "RGU E_USR\n") == 0){
        printf("Invalid user ID\n");
    }
    else if (strcmp(out, "RGU E_GRP\n") == 0){
        printf("Invalid group name\n");
    }
    else if (strcmp(out, "RGU NOK\n") == 0){
        printf("Invalid credentials\n");
    }
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: Invalid message format\n");
        return;
    }

}

void showMyGroups(){
    char in[MAX_INPUT_SIZE], *out;

    if (!loggedin){
        printf("Error: User not logged in\n");
        return;
    }

    sprintf(in, "GLM %s\n", savedUID);
    out = sendUDP(in);

    if (strcmp(out, "RGM 0\n") == 0){
        printf("No groups subscribed\n");
        return;
    }
    else if (strcmp(out, "RGM E_USR\n") == 0){
        printf("Invalid user ID\n");
        return;
    }
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: Invalid message format\n");
        return;
    }

    char Groups[3];
    int i = 4;
    int j = 0;
    while (out[i] != ' '){
        Groups[j] = out[i];
        i++; j++;
    }
    Groups[j] = '\0';

    printf("Available groups:\n");

    int nGroups = atoi(Groups);
    char GID[MAX_GID_SIZE], GName[MAX_GNAME_SIZE], MID[MAX_MID_SIZE];
    i += 1;
    for (int n = 0; n < nGroups; n++){
        sscanf(&out[i], "%s %s %s", GID, GName, MID);
        i += strlen(GID) + strlen(GName) + strlen(MID) + 3;
        printf("Group ID: %s\t\tGroup Name: %-24s\t\tLatest MID: %s\n", GID, GName, MID);
    }

}

void initSession(char *host, char * port){
    initUDP(host, port);
}

void exitSession(){
    closeUDP();
}
