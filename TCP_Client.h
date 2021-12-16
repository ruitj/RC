void initTCP(char hostName[], char port[]);
char *sendTCP(char *msg, int max);
char *readTCP();
void closeTCP();