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

int relogio=0, sem=0;
char str[255], echoBuffer[255];

void cria_socket_interno(int *vetorPortas)
{

	/*Criacao do socket UNIX para receber conexoes*/
	if ((servSockInterno = socket(AF_UNIX, SOCK_STREAM, 0))<0)
	{
		perror ("criar socket interno falhou");
		exit(1);
	}

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

	/*Escutar requisicoes*/
	if ((listen(servSockInterno, 5))<0)
	{
		perror("listen socket interno falhou");
		exit(1);
	}

	while(1){
		int done, n;

		inf *mens_int;
		sprintf(str,"");

		printf("esperando conexao interna...\n");
		t = sizeof(remote);
		if ((acceptSockInterno = accept(servSockInterno, (struct sockaddr* )&remote, &t))<0)
		{
			perror("accept socket interno falhou");
			exit(1);
		}
		printf("conectado interno!\n");
		done = 0;
		do{
			sprintf(str,"");
			printf("Interno, Buffer antes do recv %s\n", str);
			n = recv(acceptSockInterno, str, sizeof(str), 0);
			if (n<=0) {
				if (n<0) perror("recv interno falhou");
				done = 1;
			}
			
			printf("Interno, Buffer depois do recv %s\n", str);
			sscanf(str,"%d %s %d",&idf,msg,&rel);	
			
			//atualiza relógio logico
			relogio++;
			rel = max(relogio, rel);
			
			sleep(1);
			sprintf(str,"");
			sprintf(str,"%d %s %d",idf,msg,rel);
			
			printf("Sou interno e vou mandar essa mensagem para o externo %s\n", str);
			cria_cliente(vetorPortas[0], str);
			
			if (!done)
				if (send(acceptSockInterno, str, n, 0) < 0)
				{
					perror("send interno falhou");
					done = 1;
				}
		}while (!done);
		
		close(acceptSockInterno);
	}
					
}

void limpaFila(fila *f)
{
	int maiores[NUM_PROCS];
	int p, menor;
	
	for (p=0; p<NUM_PROCS; p++){
	  maiores[p] = 0;
	}
	
	nodo *no;
	no = malloc(sizeof(nodo));
	no = q_ext->inicio;
	
	inf *lixo;
	lixo = malloc(sizeof(inf));
	
	//Preencho o vetor com os maiores timestamps de cada filosofo
	while(no->prox != NULL){
	  if (no->info->rel > maiores[no->info->idf]){
	    maiores[no->info->idf] = no->info->rel;
	  }
	  no = no->prox;
	}
	
	//Encontro o menor timestamp entre os maiores valores dos filosofos
	for (p=0; p<NUM_PROCS; p++){
	  if (menor > maiores[p])
	    menor = maiores[p];
	}
	
	//Percorro a fila retirando 
	while(no->prox != NULL){
	  if (no->info->rel < menor){
	    
	    if (!(strcmp(no->info->msg,"VOP"))){
	      lixo = retira(f);
	      sem++;
	    }
	    else if (!(strcmp(no->info->msg,"POP"))){
	      if (sem>0){
		sem--;
		//se no->info->idf == ident  mando msg de retorno (GO)
	      }
	    }
	  }
	  no = no->prox;
	}
	free(no);
	free(lixo);
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
		
		sprintf(echoBuffer,"");
		
		printf("esperando conexao externa\n");
		clntLen = sizeof(echoClntAddrExt);
		if((clntSockExt = accept(servSockExt, (struct sockaddr *)&echoClntAddrExt, &clntLen))<0) {
			perror("accept externo falhou");
			exit(1);
		}
		
		printf("Tratando cliente %s\n", inet_ntoa(echoClntAddrExt.sin_addr));
		recv(clntSockExt, echoBuffer, sizeof(echoBuffer), 0);
				
		sscanf(echoBuffer,"%d %s %d",&idf,msg,&rel);
		printf("Sou externo e recebi essa msg do interno %s\n",echoBuffer);
		mens_ext = criarInfo(idf, msg, rel);
		sleep(1);
		
		//atualiza o relogio
		relogio++;
		rel = max(relogio, rel);
		mens_ext->rel = rel;
		
		//switch de tipo de msg
		if((!strcmp(mens_ext->msg, "POP")) || (!strcmp(mens_ext->msg, "VOP"))) {
			
			//Insere o nodo na fila de requisicoes
			insere(q_ext,mens_ext);
			printf("Sou externo e vou inserir essa mensagem na fila %d %s %d\n", mens_ext->idf, mens_ext->msg, mens_ext->rel);
			
			//Broadcast para todos os outros
			sprintf(echoBuffer,"");
			sprintf(echoBuffer,"%d %s %d",idf,"ACK",rel);
			printf("Sou externo e vou fazer broadcast dessa mensagem %s\n",echoBuffer);
			
			sleep(1);
			int k=1;
			for(k = 1; k<NUM_PROCS; k++) {	
				cria_cliente(vetorPortas[k],echoBuffer);
			}
			
			//Atualiza o relógio 
			relogio++;
			rel = max(relogio, rel);
			mens_ext->rel=rel;
			
			//Teste de verificacao da fila
// 			printf("----fila----\n");
// 			imprimeFila(q_ext);
// 			printf("----fila----\n");
			
		}
		else if(!strcmp(mens_ext->msg, "reqP")) {
			
			//Insere o nodo na fila de requisicoes
			//sprintf(mens_ext->msg, "%s", "POP");
			//insere(q_ext,mens_ext);
			//printf("Sou externo e vou inserir essa mensagem na fila %d %s %d\n", mens_ext->idf, mens_ext->msg, mens_ext->rel);
			
			//Broadcast para todos os outros
			sprintf(echoBuffer,"");
			sprintf(echoBuffer,"%d %s %d",idf,"POP",rel);
			printf("Sou externo e vou fazer broadcast dessa mensagem %s\n",echoBuffer);
			
			sleep(1);
			int k;
			for(k = 1; k<NUM_PROCS; k++) {
				cria_cliente(vetorPortas[k],echoBuffer);
			}
			
			//Atualiza o relogio
			relogio++;
			rel = max(relogio, rel);
			mens_ext->rel=rel;
			
			//Teste de verificacao da fila
// 			printf("----fila----\n");
// 			imprimeFila(q_ext);
// 			printf("----fila----\n");
		     
		     }
		     else if(!strcmp(mens_ext->msg, "reqV")){
			    
			    
		            //Insere o nodo na fila de requisicoes
			    //sprintf(mens_ext->msg, "%s", "VOP");
			    //insere(q_ext,mens_ext);
			    //printf("Sou externo e vou inserir essa mensagem na fila %d %s %d\n", mens_ext->idf, mens_ext->msg, mens_ext->rel);
			    
			    //Broadcast para todos os outros
			    sprintf(echoBuffer,"");
			    sprintf(echoBuffer,"%d %s %d",idf,"VOP",rel);
			    printf("Sou externo e vou fazer broadcast dessa mensagem %s\n",echoBuffer);
			    
			    sleep(1);
			    int k;
			    for(k = 1; k<NUM_PROCS; k++) {
				    cria_cliente(vetorPortas[k],echoBuffer);
			    }
			    
			    //Atualiza o relogio
			    relogio++;
			    rel = max(relogio, rel);
			    mens_ext->rel=rel;
			
			    //Teste de verificacao da fila
// 			    printf("----fila----\n");
// 			    imprimeFila(q_ext);
// 			    printf("----fila----\n");
			  }
			  else 
			  {
			      
			      
			      
			  };
		
	}	
}
