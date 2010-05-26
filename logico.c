#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "logico_sock.h"

int cliSockInterno, t, len;
struct sockaddr_un remote;
char str[100];

void comer()
{
	printf("Comendo...\n");
	srand(time(NULL));
	int num = (rand()%5)+1; 
	sleep(num); 
}

void pensar()
{
	printf("Pensando...\n");
	srand(time(NULL));
	sleep((rand()%5)+1); 
}

void pedir(int idf)
{
	sprintf(str, "%d %s %d", idf,"reqP",2);
   	if (send(cliSockInterno, str, strlen(str), 0) == -1) {
        	perror("send");
	       	exit(1);
	}
	if ((t=recv(cliSockInterno, str, 100, 0)) > 0) {
		str[t] = '\0';
	}
	else {
	       	if (t < 0) perror("recv");
	        else printf("Server closed connection\n");
	       	exit(1);
	}
}

void *filosofo(int *num_idf)
{
	int idf = (int *) num_idf;
	while(1)
	{
		printf("Thread: %d\n", idf);
       		comer();
		pensar();
		pedir(idf);
  	}
}

int main(int argc, char *argv[])
{  

	cria_sock();	
  
  	pthread_t fil;
   
	int i; 
  
  	for(i=0; i<5; i++)
    		pthread_create(&fil, NULL, (void *) filosofo, (int *) i);
  
  	pthread_join(fil, NULL);
  
  	return 0;
    
}
  
