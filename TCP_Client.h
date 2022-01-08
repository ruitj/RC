void initTCP(char hostName[], char port[]);
char *sendTCP(char *msg, int n_bytes);
void connectTCP();
int writeTCP(char *msg);
int readTCP(int n_bytes, char *content);
void closeTCP();