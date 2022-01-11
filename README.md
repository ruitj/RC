# RC
gcc -O3 -Wall UDP_Client.c TCP_Client.c func_Client.c user.c -lm -o user


void processInputTCP(int connfd, char *command){
    char input[15];

    if(strcmp(command, "PST ")==0){
        postMessageS(connfd);
    }
    else if(strcmp(command, "RTV ")==0){
        int nread = readTCP(connfd, 14, input);
        if (nread == 14)
            retrieveMessagesS(input, connfd);
        else{
            writeTCP(connfd, 5, "ERR\n");
        }
    }
    else if(strcmp(command, "ULS ")==0){
        int nread = readTCP(connfd, 3, input);
        if (nread == 3)
            listUsers_GIDS(input, connfd);
        else{
            writeTCP(connfd, 5, "ERR\n");
        }
    }
    else{
        printf("Error: wrong message format\n");
    }
}

