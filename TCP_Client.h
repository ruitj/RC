void initTCP(char hostName[], char port[]);
void connectTCP();
char *sendTCP(char *msg, int n_bytes);
char *readTCP(int n_bytes);
int writeTCP(char *msg);
void closeTCP();