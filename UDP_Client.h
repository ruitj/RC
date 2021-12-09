#ifndef UDP_H
#define UDP_H

void initUDP(char IP[], char port[]);
char *sendUDP(char *msg);
void closeUDP();

#endif