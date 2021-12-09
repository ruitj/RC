#include <Client.h>
#include <UDP_Client.h>
#include <TCP_Client.h>

int registerUser(char *UID, char *password){
    char in[], *out;
    int status;
    sprintf(in, "REG %s %s\n", UID, password);
    status = sendUDP(in, &out);
    if (status){
        return 1;
    }
    else if (strcmp(out, "RRG OK\n") != 0){
        printf("User successfully registered\n");
    }
    else if (strcmp(out, "RRG NOK\n") != 0){
        printf("Warning: User not registered\n");
    }
    else if (strcmp(out, "RRG DUP\n") != 0){
        printf("Warning: User already registered\n");
    }
    else{
        printf("Warning: wrong message format\n")
    }
    return 0;
}
