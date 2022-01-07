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

int appendtoFile(char *filename, char *content){
    FILE *fp;
    if((fp = fopen(filename, "ab+")) != NULL){
        fputs(content, fp);
        return 1;
    }
    return 0;
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
        return;
    }
    else if (strcmp(out, "RRG NOK\n") == 0){
        printf("Invalid credentials\n");
        return;
    }
    else if (strcmp(out, "RRG DUP\n") == 0){
        printf("User already registered\n");
        return;
    }
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: unexpected protocol message\n");
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
        if (strncmp(input, savedUID, 5) == 0){
            savedUID[0] = '\0';
            savedPass[0] = '\0';
            savedGID[0] = '\0';
            loggedin = 0;
            GIDSelected = 0;
        }
        printf("User successfully unregistered\n");
        return;
    }
    else if (strcmp(out, "RUN NOK\n") == 0){
        printf("Invalid credentials\n");
        return;
    }
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: unexpected protocol message\n");
        return;
    }
}

void loginUser(char *input){
    char in[MAX_INPUT_SIZE], *out;
    
    if (loggedin){
        printf("User UID=%s already logged in\n", savedUID);
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
        return;
    }
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: unexpected protocol message\n");
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
        return;
    }
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: unexpected protocol message\n");
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
        printf("Error: unexpected protocol message\n");
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

    if ((input[0] == '0') && (input[1] == ' ')){
        strcpy(GID, "00");
        GID[2] = '\0';
    }

    sprintf(in, "GSR %s %s %s\n", savedUID, GID, GName);
    out = sendUDP(in);

    if (strcmp(out, "ERR\n") == 0){
        printf("Error: unexpected protocol message\n");
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
        printf("Error: unexpected protocol message\n");
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
        printf("Error: unexpected protocol message\n");
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

}

void showGIDSelected(){
    if (!GIDSelected){
        printf("No group selected\n");
        return;
    }

    printf("Selected group: %s\n", savedGID);
}

void listUsers_GID(){
    char in[MAX_INPUT_SIZE], *out, *status;

    if (!GIDSelected){
        printf("No group selected\n");
        return;
    }

    sprintf(in, "ULS %s\n", savedGID);
    
    status = sendTCP(in, 7);

    if (strcmp(status, "RLU NOK") == 0){
        printf("Error: invalid group ID\n");
        return;
    }
    else if (strcmp(status, "ERR") == 0){
        printf("Error: unexpected protocol message\n");
        return;
    }

    char GName[MAX_GNAME_SIZE];
    int j;
    out = readTCP(1);
    for (j = 0; out[0] != ' '; j++){
        GName[j] = out[0];
        out = readTCP(1);
    }
    GName[j] = '\0';

    printf("Group name: %s\n", GName);
    printf("User IDs: ");

    while (1){
        out = readTCP(MAX_OUTTCP_SIZE-1);
        printf("%s", out);
        int size = strlen(out);
        if (out[size-1] == '\n')
            break;
    }
    closeTCP();
}

void postMessage(char *input){
    char in[MAX_INPUT_SIZE], *status, text[MAX_TEXT_SIZE], FName[MAX_FNAME_SIZE], buffer[512], input_temp[MAX_TEXT_SIZE], *sendBuffer;
    int spaceIndex=-1, withFile=0, sizeFile=0, duasAspas=0;
    ssize_t nwritten, nleft;
    FILE *fptr;

    if (!loggedin){
        printf("Error: User not logged in\n");
        return;
    }
    else if (!GIDSelected){
        printf("Error: No group selected\n");
        return;
    }
    int i = 1;
    strcpy(input_temp,input);
    if(input_temp[0] != '"'){
        printf("Error: Wrong post format\n");
        return;
    }
    while(input_temp[i] != '\n'){
        if(input_temp[i] == '"'){
            if(input_temp[i+1] == ' '){
                duasAspas = 1;
                spaceIndex = i+1;
                withFile = 1;
                break;
            }
            else if(input_temp[i+1] == '\n'){
                duasAspas = 1;
                break;
            }
            else{
                printf("Error: Wrong post format\n");
                return;
            }
        }
        text[i-1] = input_temp[i];
        i++;
    }
    text[i-1] = '\0';
    if(duasAspas == 0){
        printf("Error: Wrong post format\n");
        return;
    }
    if(withFile == 0){
        sprintf(in, "PST %s %s %lu %s\n", savedUID, savedGID, strlen(text), text);
        connectTCP();
        writeTCP(in);
    }
    else{
        strcpy(FName,&input_temp[spaceIndex+1]);
        FName[strlen(FName)-1] = '\0';
        fptr = fopen(FName,"rb");
        if (fptr == NULL){
            printf("Error: file cannot be opened\n");
            closeTCP();
            fclose(fptr);
            return;
        }
        fseek(fptr,0,SEEK_END);
        sizeFile = ftell(fptr);
        fseek(fptr,0,SEEK_SET);
        //falta enviar o ficheiro
        sprintf(in, "PST %s %s %lu %s %s %d ", savedUID, savedGID, strlen(text), text, FName, sizeFile);
        connectTCP();
        writeTCP(in);
        nleft = sizeFile;
        while(nleft>0){
            if(fread(buffer, 512, 1, fptr)){
                sendBuffer = &buffer[0];
                nwritten = writeTCP(sendBuffer);
                if(nwritten<0){
                    printf("Error: error sending file\n");
                    closeTCP();
                }
                nleft -= nwritten;
                sendBuffer += nwritten;
            }
        }
        fclose(fptr);
    }
    status = readTCP(9);

    if (strcmp(status, "RPT NOK\n") == 0){
        printf("Error: invalid post\n");
        closeTCP();
        return;
    }
    else if (strcmp(status, "ERR\n") == 0){
        printf("Error: unexpected protocol message\n");
        closeTCP();
        return;
    }
    
    status[strlen(status)-1] = '\0';
    printf("Posted message %s to group %s\n", &status[4], savedGID);
    closeTCP();
    return;
}

void retrieveMessages(char *input){
    char in[MAX_INPUT_SIZE], *out, *status;

    if (!GIDSelected){
        printf("No group selected\n");
        return;
    }
    else if (!loggedin){
        printf("Error: User not logged in\n");
        return;
    }

    for (int i = 0; input[i] != '\n'; i++){
        if ((!isdigit(input[i])) || (i >= 4)){
            printf("Error: invalid credentials\n");
            return;
        }
    }

    char MID[MAX_MID_SIZE];
    sscanf(input,"%s", MID);
    sprintf(in ,"RTV %s %s %s\n", savedUID, savedGID, MID);

    status = sendTCP(in, 7);

    if (strcmp(status, "RRT NOK") == 0){
        printf("Error: invalid credentials\n");
        return;
    }
    else if (strcmp(status, "RRT EOF") == 0){
        printf("No messages available\n");
        return;
    }
    else if (strcmp(status, "ERR\n") == 0){
        printf("Error: unexpected protocol message\n");
        return;
    }

    out = readTCP(3); // reads number of messages retrieved
    printf("%s message(s) retrieved:\n", out);
    int n_msgs = atoi(out);
    int n_read = 0; // msgs already read
    int j;

    readTCP(1); // reads space
    out = readTCP(1);
    while (n_read < n_msgs){
        char MID[MAX_MID_SIZE], TSize[4], text[MAX_TEXT_SIZE];

        for (j = 0; isdigit(out[0]); j++){
            MID[j] = out[0];
            out = readTCP(1);
        }
        MID[j] = '\0';

        readTCP(6); // reads UID

        out = readTCP(1);
        for (j = 0; isdigit(out[0]); j++){
            TSize[j] = out[0];
            out = readTCP(1);
        }
        TSize[j] = '\0';

        int size = atoi(TSize);
        out = readTCP(size);
        strcpy(text, out);
        text[size] = '\0'; // remove \n

        printf("%s - \"%s\"", MID, text);

        readTCP(1); // reads space
        out = readTCP(1);
        
        if (out[0] == '/'){
            readTCP(1); // reads space
            char FName[MAX_FNAME_SIZE], FSize[11];

            out = readTCP(1);
            for (j = 0; out[0] != ' '; j++){
                FName[j] = out[0];
                out = readTCP(1);
            }
            FName[j] = '\0';

            out = readTCP(1);
            for (j = 0; out[0] != ' '; j++){
                FSize[j] = out[0];
                out = readTCP(1);
            }
            FSize[j] = '\0';

            int size = atoi(FSize);
            char *content;
            while (size > MAX_OUTTCP_SIZE){
                content = readTCP(MAX_OUTTCP_SIZE);
                if (!appendtoFile(FName, content)){
                    printf("Error: unable to store file\n");
                    exit(1);
                }
                size -= MAX_OUTTCP_SIZE;
            }
            content = readTCP(size);
            if (!appendtoFile(FName, content)){
                printf("Error: unable to store file\n");
                exit(1);
            }
            readTCP(1);

            printf("; file stored: %s", FName);

            out = readTCP(1);

        }
        printf("\n");
        n_read++;
    }

    closeTCP();
}

void initSession(char *hostName, char *port){
    initUDP(hostName, port);
}

void exitSession(){
    closeUDP();
}
