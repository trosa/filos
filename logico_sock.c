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

#include "logico.h"
#include "logico_sock.h"

void cria_sock() {

	if ((cliSockInterno = socket(AF_UNIX, SOCK_STREAM, 0))<0) {
		perror("criacao socket falhou");
		exit(1);
	}
	
	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, "echo_socket");
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	
	if ((connect(cliSockInterno, (struct sockaddr *)&remote, len))<0){
		perror("connect socket falhou");
		exit(1);
	}

	printf("Conectado!\n");

}
