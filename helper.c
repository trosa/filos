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

#include "helper.h"
#include "helper_sock.h"
#include "fila.h"

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
int garfo, idf, rel, msg;
int meu_idf;

void *socket_interno(int *vetorPortas)
{
	cria_socket_interno(vetorPortas);
}

void *socket_externo(int *vetorPortas)
{

	cria_socket_externo(vetorPortas);
}

int main(int argc, char *argv[])
{
	pthread_t extSocket, intSocket;
	int vetorPortas[NUM_PROCS];
	
	/* checagem de parametros */
	if (argc != NUM_PROCS+2) {
		fprintf(stderr, "%s <idf> <porta_externa>\n", argv[0]);
		exit(1);
	}

	meu_idf = atoi(argv[1]);

	//Inicializa o vetor de portas
	int i;
	
	for(i=1; i<=NUM_PROCS; i++){
	  vetorPortas[i-1] = atoi(argv[i+1]);
	}

// 	numeroPortaExterna = atoi (argv[1]);

	pthread_create(&intSocket, NULL, (void *)socket_interno, (int *) vetorPortas);
	pthread_create(&extSocket, NULL, (void *)socket_externo, (int *) vetorPortas);

	pthread_join(extSocket, NULL);
	pthread_join(intSocket, NULL);
}
