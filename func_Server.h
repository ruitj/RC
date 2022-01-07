#include "Server.h"

char *processInput(char *input);
char *processInputTCP(int connfd);
char *readnbytes(int connfd, int nbytes, char *socketread);
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
