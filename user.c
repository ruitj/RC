#include <string.h>
#include <stdio.h>
#include <Client.h>
#include <UDP_Client.h>
#include <TCP_Client.h>
#include <func_Client.h>

char port[MAX_PORT_SIZE], IP[20], savedUID[MAX_UID_SIZE];
bool login=false;
 
void processInput(){
    char input[MAX_INPUT_SIZE], optype[MAX_OPTYPE_SIZE];
    int i;

    while (fgets(input, MAX_INPUT_SIZE, stdin)){
        for (i = 0; strcmp(input[i], " ") != 0; i++)
            optype[i] = input[i];
        if (strcmp(optype, "reg") == 0){
            char UID[MAX_UID_SIZE], password[MAX_PASS_SIZE];
            if (sscanf(&input[4],"%s %s",UID,password)==2){
                if (registerUser(UID, password))
                    fprintf(stderr, "Error: UDP connection failure\n");
            }
        }
        else if ((strcmp(optype, "unr") == 0) || (strcmp(optype, "unregister") == 0)){
            string UID, password;
            if(scanf("%s %s",UID,password)==2){
                sprintf(stringout, "UNR %s %s\n",UID,password);
                //sendUDP(stringout);
            }
            else{
                fprintf(stderr,"Error: wrong input format\n");
                }
        }
        else if (strcmp(optype, "login") == 0){
            string UID, password;
            if(scanf("%s %s",UID,password)==2){
                sprintf(stringout, "LOG %s %s\n",UID,password);
                //sendUDP(stringout);
                login = true;
                savedUID = UID;
            }
            else{
                fprintf(stderr,"Error: wrong input format\n");
            }
        }
        else if (strcmp(optype, "logout") == 0){
            stringout = "OUT";
            //sendUDP(stringout);
            login = false;
            savedUID = NULL;
        }
        else if (strcmp(optype, "exit") == 0){
            //closeTCP
        }
        else if ((strcmp(optype, "groups") == 0) || (strcmp(optype, "gl") == 0)){
            stringout = "GLS";
            //sendUDP(stringout);
        }
        else if ((strcmp(optype, "ulist") == 0) || (strcmp(optype, "ul") == 0)){
            string GID;
            if(scanf("%s",GID)==1){
                sprintf(stringout, "ULS %s\n",GID);
                //sendTCP(stringout);
            }
            else{
                fprintf(stderr,"Error: wrong input format\n");
            }
        }
        else if ((strcmp(optype, "subscribe") == 0) || (strcmp(optype, "s") == 0)){
            string GID, GName;
            if(scanf("%s %s",GID,GName)==2){
                sprintf(stringout, "GSR %s %s %s\n",savedUID,GID,GName);
                //sendUDP(stringout);
            }
            else{
                fprintf(stderr,"Error: wrong input format\n");
            }
        }
        else if ((strcmp(optype, "unsubscribe") == 0) || (strcmp(optype, "u") == 0)){
            string GID;
            if(scanf("%s",GID)==1){
                sprintf(stringout, "GUR %s %s\n",savedUID,GID);
                //sendUDP(stringout);
            }
            else{
                fprintf(stderr,"Error: wrong input format\n");
            }
        }
        else if ((strcmp(optype, "my_groups") == 0) || (strcmp(optype, "mgl") == 0)){
            sprintf(stringout, "GLM %s\n",savedUID);
            //sendUDP(stringout);
        }
        else if ((strcmp(optype, "select") == 0) || (strcmp(optype, "sag") == 0)){
            string GID;
            if(scanf("%s",GID)!=1){
                fprintf(stderr,"Error: wrong input format\n");
            }
        }
        else if (strcmp(optype, "post") == 0){
            string text, Fname;
            if(scanf("%s %s",text,Fname)==2){
                //sprintf(stringout, "PST %s %s %d %s %d %s\n",savedUID,GID,strlen(text),text,,FILE);
                //sendUDP(stringout);
            }
            else{
                fprintf(stderr,"Error: wrong input format\n");
            }
        }
        else if ((strcmp(optype, "retrieve") == 0) || (strcmp(optype, "r") == 0)){
            // do stuff
        }
        else { /* default case */
            fprintf(stderr, "Error: wrong input format\n");
        }
    }

}

int parseArgs(int n, char **args){
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
                    exit(EXIT_FAILURE);
            }
    }
}

int main(int argc, char**argv){

    //input parsing
    //parseArgs(argc, argv)
    IP = "tejo.tecnico.ulisboa.pt"
    port = PORT_DEFAULT

    initUDP(IP, port);

    processInput();

    return 0;
}
