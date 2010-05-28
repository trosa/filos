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
#include "criacliente.h"

void cria_socket_interno(int *vetorPortas)
{

	/*Criacao do socket UNIX para receber conexoes*/
	if ((servSockInterno = socket(AF_UNIX, SOCK_STREAM, 0))<0)
	{
		perror ("criar socket interno falhou");
		exit(1);
	}
	//printf("socket interno criado... %d\n", servSockInterno);

	/*bind do socket descriptor para um endereco no Unix*/
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path,"echo_socket");
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if ((bind(servSockInterno, (struct sockaddr *)&local, len))<0)
	{
		perror("bind socket interno falhou");
		exit(1);
	}
	//printf("bind interno feito...\n");

	/*Escutar requisicoes*/
	if ((listen(servSockInterno, 5))<0)
	{
		perror("listen socket interno falhou");
		exit(1);
	}
	//printf("listen interno feito...\n");
	
	//Criacao da fila de mensagens
	fila *q_int;
	q_int = criaFila();
	
	while(1) {
		int done, n;

		inf *mens_int;

		printf("esperando conexao interna...\n");
		t = sizeof(remote);
		if ((acceptSockInterno = accept(servSockInterno, (struct sockaddr* )&remote, &t))<0)
		{
			perror("accept socket interno falhou");
			exit(1);
		}
		printf("conectado interno!\n");
		done = 0;
		do {
			n = recv(acceptSockInterno, str, 100, 0);
			if (n<=0) {
				if (n<0) perror("recv interno falhou");
				done = 1;
			}
			sscanf(str,"%d %s %d",&idf,msg,&rel);
			//printf("%d,%s,%d\n",idf,msg,rel);
			mens_int = criarInfo(idf, msg, rel);
			
			//switch de tipo de msg
			if(!strcmp(mens_int->msg, "reqP")){
			  int k;
			  for(k=0; k<NUM_PROCS; k++){
			    printf("Sou interno, recebi a mensagem do logico %s\n", str);
			    //sprintf(str,"");
			    sprintf(str, "%d %s %d", idf, "POP", rel);
// 			    printf("Sou interno, mandei %s para %d\n", str, vetorPortas[k]); 
			    cria_cliente(vetorPortas[k], str);
			  }
			}
			else if(!strcmp(mens_int->msg, "reqV")) {
				int k;
				for(k = 0; k<NUM_PROCS; k++) {
					printf("Sou interno, recebi a mensagem do logico %s\n", str);
					//sprintf(str,"");
					sprintf(str, "%d %s %d", idf, "VOP", rel);
					printf("Sou interno, mandei %s para %d\n",str, vetorPortas[k]); 
					cria_cliente(vetorPortas[k], str);
				}
			}  
			
// 			printf("Nodo interno criado: %d\n",idf);
// 			insere(q_int,mens_int);	
			
			if (!done)
				if (send(acceptSockInterno, str, n, 0) < 0)
				{
					perror("send interno falhou");
					done = 1;
				}
		} while (!done);
		
		close(acceptSockInterno);
	}
					
}

void cria_socket_externo(int *vetorPortas) {
	int l;
	 
	//Criando socket tcp para esperar conexoes
	if ((servSockExt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
		perror("criacao socket interno falhou");
		exit(1);
	}
	//printf("socket externo criado... %d\n", servSockExt);

	//Construindo endereco de conexao
	memset(&echoServAddrExt, 0, sizeof(echoServAddrExt));
	echoServAddrExt.sin_family = AF_INET;
	echoServAddrExt.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServAddrExt.sin_port = htons(vetorPortas[0]);

	//Bind ao endereco local
	if (bind(servSockExt, (struct sockaddr *)&echoServAddrExt, sizeof(echoServAddrExt))<0) {
		perror("bind externo falhou");
		exit(1);
	}
	//printf("bind externo feito...\n");
	
	//Fazendo listen
	if (listen(servSockExt,5)<0) {
		perror("listen externo falhou");
		exit(1);
	}
	//printf("listen externo feito\n");
	
	//Criacao da fila de mensagens
	fila *q_ext;
	q_ext = criaFila();

	//Esperando conexoes com accept
	while(1) {
		
		inf *mens_ext;
		
		printf("esperando conexao externa\n");
		clntLen = sizeof(echoClntAddrExt);
		if((clntSockExt = accept(servSockExt, (struct sockaddr *)&echoClntAddrExt, &clntLen))<0) {
			perror("accept externo falhou");
			exit(1);
		}
		
		printf("Tratando cliente %s\n", inet_ntoa(echoClntAddrExt.sin_addr));
		recv(clntSockExt, echoBuffer, sizeof(echoBuffer), 0);
		
		sscanf(echoBuffer,"%d %s %d",&idf,msg,&rel);
		//printf("%d,%s,%d\n",idf,msg,rel);
		//sprintf(echoBuffer,"");
		mens_ext = criarInfo(idf, msg, rel);
		
		
		//switch de tipo de msg
		if((!strcmp(mens_ext->msg, "POP")) || (!strcmp(mens_ext->msg, "VOP"))) {
			//printf("o que eu recebi foi %s\n",mens_ext->msg);
			
			printf("Vou tentar inserir %s\n", mens_ext->msg);
			insere(q_ext,mens_ext);

			printf("----fila----\n");
			imprimeFila(q_ext);
			printf("----fila----\n");

			int k;
			for(k = 0; k<NUM_PROCS; k++) {
				
				sprintf(echoBuffer,"%d %s %d",idf,"ACK",rel);
				cria_cliente(vetorPortas[k],echoBuffer);
			}
		}
		else {
			printf("sou externo %d, recebi ACK, faco o que?\n",echoServAddrExt.sin_port);
		}
		
// 		printf("Nodo externo criado: %d\n",idf);
// 		insere(q_ext,mens_ext);			
		
		send(clntSockExt, echoBuffer, sizeof(echoBuffer), 0);
	}	
}
