#include "Client.h"
#include "UDP_Client.h"
#include "TCP_Client.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char savedUID[MAX_UID_SIZE], savedPass[MAX_PASS_SIZE];
char savedGID[MAX_GID_SIZE];
int loggedin = 0, GIDSelected = 0;

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

int validPost(char *input){

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
    sscanf(input, "%s %s", savedUID, savedPass);
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
    savedGID[0] = '\0';
    GIDSelected = 0;
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
    sscanf(&out[4], "%s", Groups);

    int nGroups = atoi(Groups);
    char GID[MAX_GID_SIZE], GName[MAX_GNAME_SIZE], MID[MAX_MID_SIZE];
    
    printf("Available groups:\n");
    for (int n = 0, i = 6; n < nGroups; n++){
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

    if (strcmp(out, "ERR\n") == 0){
        printf("Error: Invalid message format\n");
        return;
    }
    
    char status[8];
    sscanf(&out[4], "%s", status);

    if (strcmp(status, "NEW") == 0){
        sscanf(&out[8], "%s", GID);
        printf("New group created and subscribed: %s - \"%s\"\n", GID, GName);
    }
    else if (strcmp(status, "OK") == 0){
        sscanf(&out[7], "%s", GID);
        printf("New group subscribed: %s - \"%s\"\n", GID, GName);
    }
    else if (strcmp(status, "NOK") == 0){
        printf("Invalid credentials\n");
    }
    else if (strcmp(status, "E_USR") == 0){
        printf("Invalid user ID\n");
    }
    else if (strcmp(status, "E_GRP") == 0){
        printf("Invalid group ID\n");
    }
    else if (strcmp(status, "E_GNAME") == 0){
        printf("Invalid group name\n");
    }
    else if (strcmp(status, "E_FULL") == 0){
        printf("Maximum number of groups achieved\n");
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
    sscanf(&out[4], "%s", Groups);

    int nGroups = atoi(Groups);
    char GID[MAX_GID_SIZE], GName[MAX_GNAME_SIZE], MID[MAX_MID_SIZE];
    
    printf("Subscribed groups:\n");
    for (int n = 0, i = 6; n < nGroups; n++){
        sscanf(&out[i], "%s %s %s", GID, GName, MID);
        i += strlen(GID) + strlen(GName) + strlen(MID) + 3;
        printf("Group ID: %s\t\tGroup Name: %-24s\t\tLatest MID: %s\n", GID, GName, MID);
    }

}

void selectGroup(char *input){

    if (!loggedin){
        printf("Error: User not logged in\n");
        return;
    }
    else if (!validGID(input)){
        printf("Error: Invalid input format\n");
        return;
    }

    sscanf(input, "%s", savedGID);
    GIDSelected = 1;
    printf("Group %s is now the active group\n", savedGID);

}a

void post(char *input){
    char in[MAX_INPUT_SIZE], *out, text[MAX_TEXT_SIZE], text_tmp[MAX_TEXT_SIZE], FName[MAX_FNAME_SIZE];
    int spaceIndex, withFile = 0;
    FILE *fptr;

    if (!loggedin){
        printf("Error: User not logged in\n");
        return;
    }
    /*else if (!validPost(input)){
        printf("Error: Wrong post format\n");
        return;
    }*/
    //copiar texto sem as aspas e posicionar spaceIndex no espaco
    int i = 0, j = 0;
    for(i; input[i] != '\n'; i++){
        if(input[i] != '"'){
            text[j] = input[i];
            j++;
        }
        else if(input[i] == ' '){
            spaceIndex = i;
            withFile = 1;
        }
    }
    text[j] = '\n';

    if((input[0] != '"') || (input[spaceIndex-1] != '"')){
        printf("Error: Wrong post format\n");
        return;
    }

    if(withFile == 0){
        sprintf(in, "PST %s %s %d %s\n", savedUID, savedGID, strlen(text), text);
    }
    else{
        j=0;
        for(i++;input[i] != '\n';i++){
            if(input[i] == ' '){
                printf("Error: Wrong post format\n");
                return;
            }
            FName[j] = input[i];
            j++;
        }
        FName[j] = '\n';

        fptr = fopen(Fname,"rb");

        sprintf(in, "PST %s %s %d %s %s %d %s\n", savedUID, savedGID, strlen(text), text, Fname, strlen(),);
    }

    out = sendTCP(in);
    while (1){
        char *buffer = readTCP();
        printf("%s", buffer);
        if(strlen(buffer) == MAX_OUTPUT_SIZE-1)
            break;
    }
    closeFD();
}

void initSession(char *host, char *port){
    initUDP(host, port);
    //initTCP(host, port);
}

void exitSession(){
    closeUDP();
    //closeTCP();
}
