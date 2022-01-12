#include "Client.h"
#include "UDP_Client.h"
#include "TCP_Client.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char savedUID[MAX_UID_SIZE], savedPass[MAX_PASS_SIZE];
char savedGID[MAX_GID_SIZE];
int loggedin = 0, GIDSelected = 0;
char buffer_tcp[MAX_OUTTCP_SIZE+1];

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
        printf("Error: unexpected protocol message sent\n");
        return;
    }
    else{
        printf("Error: unexpected protocol message received\n");
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
        printf("Error: unexpected protocol message sent\n");
        return;
    }
    else{
        printf("Error: unexpected protocol message received\n");
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
        // save credentials (UID and Password)
        sscanf(input, "%s %s", savedUID, savedPass);
        loggedin = 1;
    }
    else if (strcmp(out, "RLO NOK\n") == 0){
        printf("Invalid credentials\n");
        return;
    }
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: unexpected protocol message sent\n");
        return;
    }
    else{
        printf("Error: unexpected protocol message received\n");
        return;
    }

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
        // forget credentials (UID and Password)
        savedUID[0] = '\0';
        savedPass[0] = '\0';
        savedGID[0] = '\0';
        GIDSelected = 0;
        loggedin = 0;
    }
    else if (strcmp(out, "ROU NOK\n") == 0){
        printf("Invalid credentials\n");
        return;
    }
    else if (strcmp(out, "ERR\n") == 0){
        printf("Error: unexpected protocol message sent\n");
        return;
    }
    else{
        printf("Error: unexpected protocol message received\n");
        return;
    }

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
        printf("Error: unexpected protocol message sent\n");
        return;
    }
    else if (strncmp(out, "RGL ", 4) != 0){
        printf("Error: unexpected protocol message received\n");
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
        printf("Error: unexpected protocol message sent\n");
        return;
    }
    else if (strncmp(out, "RGS ", 4) != 0){
        printf("Error: unexpected protocol message received\n");
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
        printf("Error: unexpected protocol message sent\n");
        return;
    }
    else{
        printf("Error: unexpected protocol message received\n");
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
        printf("Error: unexpected protocol message sent\n");
        return;
    }
    else if (strncmp(out, "RGM ", 4) != 0){
        printf("Error: unexpected protocol message received\n");
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
    char in[MAX_INPUT_SIZE];

    if (!GIDSelected){
        printf("No group selected\n");
        return;
    }

    sprintf(in, "ULS %s\n", savedGID);
    
    connectTCP();
    writeTCP(in, strlen(in));
    readTCP(7, buffer_tcp);

    if (strcmp(buffer_tcp, "RUL NOK") == 0){
        printf("Error: invalid group ID\n");
        return;
    }
    else if (strcmp(buffer_tcp, "ERR") == 0){
        printf("Error: unexpected protocol message sent\n");
        return;
    }
    else if (strncmp(buffer_tcp, "RUL OK ", 7) != 0){
        printf("Error: unexpected protocol message received\n");
        return;
    }

    char GName[MAX_GNAME_SIZE];
    int j;
    readTCP(1, buffer_tcp);
    for (j = 0; buffer_tcp[0] != ' '; j++){
        GName[j] = buffer_tcp[0];
        readTCP(1, buffer_tcp);
    }
    GName[j] = '\0';

    printf("Group name: %s\n", GName);
    printf("User IDs: ");

    while (1){
        int nread = readTCP(MAX_OUTTCP_SIZE, buffer_tcp);
        printf("%s", buffer_tcp);
        if (buffer_tcp[nread-1] == '\n')
            break;
    }
    closeTCP();
}

void postMessage(char *input){
    char in[MAX_INPUT_SIZE], text[MAX_TEXT_SIZE], FName[MAX_FNAME_SIZE], buffer_post[513];
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
    if(input[0] != '"'){
        printf("Error: Wrong post format\n");
        return;
    }
    while(input[i] != '\n'){
        if(input[i] == '"'){
            if(input[i+1] == ' '){
                duasAspas = 1;
                spaceIndex = i+1;
                withFile = 1;
                break;
            }
            else if(input[i+1] == '\n'){
                duasAspas = 1;
                break;
            }
            else{
                printf("Error: Wrong post format\n");
                return;
            }
        }
        text[i-1] = input[i];
        if (i > 241){
            printf("Error: text too long\n");
            return;
        }
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
        writeTCP(in,strlen(in));
    }
    else{
        i = 1;
        while(input[spaceIndex+i] != '\n'){
            if (i > 24){
                printf("Error: filename too long\n");
                return;
            }
            FName[i-1] = input[spaceIndex+i];
            i++;
        }
        FName[i-1] = '\0';
        fptr = fopen(FName,"rb");
        if (fptr == NULL){
            printf("Error: file cannot be opened\n");
            closeTCP();
            return;
        }
        fseek(fptr,0,SEEK_END);
        sizeFile = ftell(fptr);
        fseek(fptr,0,SEEK_SET);
        sprintf(in, "PST %s %s %lu %s %s %d ", savedUID, savedGID, strlen(text), text, FName, sizeFile);
        connectTCP();
        writeTCP(in,strlen(in));
        nleft = sizeFile;
        while(nleft>0){
            buffer_post[0] = '\0';
            if(nleft > 512){
                if (!fread(buffer_post, 1, 512, fptr))
                    return;
                buffer_post[512] = '\0';
                nwritten = writeTCP(buffer_post,512);
                if(nwritten <= 0){
                    printf("Error: error sending file\n");
                    closeTCP();
                }
            }
            else{
                if (!fread(buffer_post, 1, nleft, fptr))
                    return;
                buffer_post[nleft] = '\0';
                nwritten = writeTCP(buffer_post,nleft);
            }
            nleft -= nwritten;
        }
        fclose(fptr);
    }
    readTCP(9,buffer_tcp);

    if (strcmp(buffer_tcp, "RPT NOK\n") == 0){
        printf("Error: invalid post\n");
        closeTCP();
        return;
    }
    else if (strcmp(buffer_tcp, "ERR\n") == 0){
        printf("Error: unexpected protocol message sent\n");
        closeTCP();
        return;
    }
    else if (strncmp(buffer_tcp, "RPT ", 4) != 0){
        printf("Error: unexpected protocol message received\n");
        closeTCP();
        return;
    }

    buffer_tcp[strlen(buffer_tcp)-1] = '\0';
    printf("Posted message %s to group %s\n", &buffer_tcp[4], savedGID);
    closeTCP();
    return;
}

void retrieveMessages(char *input){
    char in[MAX_INPUT_SIZE];

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

    connectTCP();
    writeTCP(in, strlen(in));
    readTCP(7, buffer_tcp);

    if (strcmp(buffer_tcp, "RRT NOK") == 0){
        printf("invalid credentials\n");
        return;
    }
    else if (strcmp(buffer_tcp, "RRT EOF") == 0){
        printf("No messages available\n");
        return;
    }
    else if (strcmp(buffer_tcp, "ERR\n") == 0){
        printf("Error: unexpected protocol message sent\n");
        return;
    }
    else if (strncmp(buffer_tcp, "RRT ", 4) != 0){
        printf("Error: unexpected protocol message received\n");
        return;
    }

    char msgs[5];
    int i;
    readTCP(1, buffer_tcp); // reads number of messages retrieved
    for (i = 0; isdigit(buffer_tcp[0]); i++){
        msgs[i] = buffer_tcp[0];
        readTCP(1, buffer_tcp);
    }
    msgs[i] = '\0';
    
    printf("%s message(s) retrieved:\n", msgs);
    int n_msgs = atoi(msgs);
    int n_read = 0; // msgs already read
    int j;

    readTCP(1, buffer_tcp);
    while (n_read < n_msgs){
        char MID[MAX_MID_SIZE], TSize[4], text[MAX_TEXT_SIZE];

        for (j = 0; isdigit(buffer_tcp[0]); j++){
            MID[j] = buffer_tcp[0];
            readTCP(1, buffer_tcp);
        }
        MID[j] = '\0';

        readTCP(6, buffer_tcp); // reads UID

        readTCP(1, buffer_tcp);
        for (j = 0; isdigit(buffer_tcp[0]); j++){
            TSize[j] = buffer_tcp[0];
            readTCP(1, buffer_tcp);
        }
        TSize[j] = '\0';

        int size = atoi(TSize);
        readTCP(size, buffer_tcp);
        strcpy(text, buffer_tcp);
        text[size] = '\0';

        printf("%s - \"%s\"", MID, text);

        readTCP(1, buffer_tcp); // reads space
        readTCP(1, buffer_tcp);
        
        if (buffer_tcp[0] == '/'){
            readTCP(1, buffer_tcp); // reads space
            char FName[MAX_FNAME_SIZE], FSize[11];

            readTCP(1, buffer_tcp);
            for (j = 0; buffer_tcp[0] != ' '; j++){
                FName[j] = buffer_tcp[0];
                readTCP(1, buffer_tcp);
            }
            FName[j] = '\0';

            readTCP(1, buffer_tcp);
            for (j = 0; buffer_tcp[0] != ' '; j++){
                FSize[j] = buffer_tcp[0];
                readTCP(1, buffer_tcp);
            }
            FSize[j] = '\0';

            int size = atoi(FSize);
            FILE *fp = fopen(FName, "wb");
            while (size > 0){
                int nread;
                if (size > MAX_OUTTCP_SIZE)
                    nread = readTCP(MAX_OUTTCP_SIZE, buffer_tcp);
                else
                    nread = readTCP(size, buffer_tcp);
                if (!fwrite(buffer_tcp, 1, nread, fp)){
                    printf("Error: unable to store file\n");
                    exit(1);
                }
                size -= nread;
            }
            fclose(fp);

            readTCP(1, buffer_tcp);
            printf("; file stored: %s", FName);

            readTCP(1, buffer_tcp);

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
