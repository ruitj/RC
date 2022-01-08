#include "Server.h"

char *processInput(char *input);
char *processInputTCP(int connfd, char *command);
int readTCP(int connfd, int n_bytes, char *content);
char *postMessageS(int connfd);
char *registerUserS(char *input);
char* unregisterUserS(char *input);
char *loginUserS(char *input);
char *logoutUserS(char *input);
char *showAvailableGroupsS();
char *subscribeGroupS(char *input);
char *unsubscribeGroupS(char *input);
char *showMyGroupsS(char *input);
int getIntLength(int i);
void listUsers_GIDS(char *input,int connfd);