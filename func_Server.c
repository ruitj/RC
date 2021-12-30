#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* processInput(char *input){
    char command[4], *out;
    strncpy(command, input, 3);
    command[3] = '\0';

    if (strcmp(command, "REG") == 0){
        out = registerUserS();
    }
    else if (strcmp(command, "UNR") == 0){
        out = unregisterUserS(input);
    }
    else if (strcmp(command, "LOG") == 0){
        out = loginUserS();
    }
    else if (strcmp(command, "OUT") == 0){
        out = logoutUserS();
    }
    else if (strcmp(command, "GLS") == 0){
        out = showAvailableGroupsS();
    }
    else if (strcmp(command, "GSR") == 0){
        out = subscribeGroupS();
    }
    else if (strcmp(command, "GUR") == 0){
        out = unsubscribeGroupS();
    }
    else if (strcmp(command, "GLM") == 0){
        out = showMyGroupsS();
    }
    else if (strcmp(command, "ULS") == 0){
        out = listUsers_GIDS();
    }
    else if (strcmp(command, "PST") == 0){
        out = postMessageS();
    }
    else if (strcmp(command, "RTV") == 0){
        out = retrieveMessagesS();
    }
    return out;
}
