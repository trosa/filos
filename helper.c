#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

#include "helper_sock.h"

void DieWithError(char *errorMessage);

//Variaveis do Socket Interno
int servSockInterno, acceptSockInterno, len, t;
struct sockaddr_un local, remote;
char str[100];

//Variaveis do Socket Externo
void HandleTCPClient(int clntSockExt);
int servSockExt;
int clntSockExt;
struct sockaddr_in echoServAddrExt;
struct sockaddr_in echoClntAddrExt;
unsigned int clntLen;
char echoBuffer[100];
int numeroPortaExterna;

//Variaveis comuns
int idf, rel;
char msg[255];

void *socket_interno(void *arg)
{
	cria_socket_interno();		
}

void *socket_externo(void *arg)
{	
	cria_socket_externo();
}

int main(int argc, char *argv[])
{
    pthread_t extSocket, intSocket;
    
    /* checagem de parametros */
    if (argc != 2) {
        fprintf(stderr, "%s <porta_externa>\n", argv[0]);
        exit(1);
    }
    
    numeroPortaExterna = atoi (argv[1]);
    
    pthread_create(&intSocket, NULL, (void *)socket_interno, (void *) NULL);
    pthread_create(&extSocket, NULL, (void *)socket_externo, (void *) NULL);

    pthread_join(extSocket, NULL);
    pthread_join(intSocket, NULL);
}
