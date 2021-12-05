#include <string>
#include <iostream>
#include <stdio>
#define portDefault "58033"
#define IPDefault
using namespace std

string port,IP,UID;
bool login=false;
 
void processInput(){
    string input,optype,password,stringout,GName,GID;

    while(getline(cin,input)){
        sscanf(input,"%s",optype);
        if (strcmp(optype, "reg") == 0){
            if(scanf("%s %s",UID,password)==2){
                sprintf(stringout, "REG %s %s\n",UID,password);
                sendUDP(stringout);
            }
            else{
                fprintf(stderr,"Error: wrong input format\n");
                }
        }
        else if ((strcmp(optype, "unr") == 0) || (strcmp(optype, "unregister") == 0)){
            if(scanf("%s %s",UID,password)==2){
                sprintf(stringout, "UNR %s %s\n",UID,password);
                sendUDP(stringout);
            }
            else{
                fprintf(stderr,"Error: wrong input format\n");
                }
        }
            case "login":
                if(scanf("%s %s",UID,password)==2){
                        sprintf(stringout, "LOG %s %s\n",UID,password);
                        sendUDP(stringout);
                        login = true;
                }
                else{
                    fprintf(stderr,"Error: wrong input format\n");
                }
            case "logout":
                stringout = "OUT"
                sendUDP(stringout);
                login = false;
            case "exit":
                //closeTCP
            case "groups":
                case "gl":
                    stringout = "GLS"
                    sendUDP(stringout);
            case "ulist":
                case "ul":
                    if(scanf("%s",GID)==1){
                        sprintf(stringout, "ULS %s\n",GID);
                        sendTCP(stringout);
                    }
                    else{
                        fprintf(stderr,"Error: wrong input format\n");
                    }
            case "subscribe":
                case "s":
                    if(scanf("%s %s",GID,GName)==2){
                            sprintf(stringout, "GSR %s %s %s\n",UID,GID,GName);
                            sendUDP(stringout);
                    }
                    else{
                        fprintf(stderr,"Error: wrong input format\n");
                    }
            case "unsubscribe":
                case "u":
                    if(scanf("%s",GID)==1){
                            sprintf(stringout, "GUR %s %s\n",UID,GID);
                            sendUDP(stringout);
                    }
                    else{
                        fprintf(stderr,"Error: wrong input format\n");
                    }
            case "my_groups":
                case "mgl":
                    sprintf(stringout, "GLM %s\n",UID);
                    sendUDP(stringout);
            case "select":   
                case "sag":
                    if(scanf("%s",GID)!=1){
                        fprintf(stderr,"Error: wrong input format\n");
                    }
            case "post":
                string text, Fname;
                if(scanf("%s %s",text,Fname)==2){
                        sprintf(stringout, "PST %s %s %d %s %d %s\n",UID,GID,strlen(text),text,,FILE);
                        sendUDP(stringout);
                    }
                    else{
                        fprintf(stderr,"Error: wrong input format\n");
                    }
            case "retrieve":
            case "r":
        }
    }

}

int main(int argc, char**argv){
    port = portDefault;
    IP = IPDefault;
    //input parsing
    if(argc > 1){
        if(strcmp(argv[1],"-n") == 0){
            IP = argv[2];
            if((argc == 5) && (strcmp(argv[3],"-p") == 0)){
                port = argv[4];
            }
            else{
                fprintf(stderr,"Error: invalid arguments\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[1],"-p") == 0){
            port = argv[2];
        }
        else{
                fprintf(stderr,"Error: invalid arguments\n");
                exit(EXIT_FAILURE);
        }
    }

    processInput();


    return 0;
}
