all: DS user
 
DS: func_Server.c DS.c   
	gcc -O3 -Wall  func_Server.c DS.c -lm -o DS
user: UDP_Client.c TCP_Client.c func_Client.c 
	gcc -O3 -Wall  UDP_Client.c TCP_Client.c func_Client.c user.c -lm -o user

