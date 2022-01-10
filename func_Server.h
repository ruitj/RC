#include "Server.h"

char *processInput(char *input);
void processInputTCP(int connfd, char *command);
int readTCP(int connfd, int n_bytes, char *content);
int writeTCP(int connfd, int n_bytes, char *content);
char *registerUserS(char *input);
char *unregisterUserS(char *input);
char *loginUserS(char *input);
char *logoutUserS(char *input);
char *showAvailableGroupsS();
char *subscribeGroupS(char *input);
char *unsubscribeGroupS(char *input);
char *showMyGroupsS(char *input);
void listUsers_GIDS(char *input,int connfd);
void postMessageS(int connfd);
void retrieveMessagesS(char *input, int connfd);
