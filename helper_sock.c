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
#include <sys/stat.h>
#include <linux/stat.h>

#include "helper.h"
#include "helper_sock.h"
#include "fila.h"
#include "criacliente.h"

#define FIFO_FILE "MYFIFO"

int relogio=0, sem=0;
char str[255], echoBuffer[255];

void cria_socket_interno(int *vetorPortas)
{
	FILE *fp;
	char readBuf[100]; char testeBuf[100];

	umask(0);
	mknod(FIFO_FILE, S_IFIFO|0666, 0);

	while(1) {
		//atualiza relógio logico
		relogio++;
		rel = max(relogio, rel);

		strcpy(testeBuf, readBuf);
	
		fp = fopen(FIFO_FILE, "r");
		fgets(readBuf, 80, fp);
		while(!strcmp(testeBuf,readBuf)) {
				fclose(fp);
				fp = fopen(FIFO_FILE, "r");
				fgets(readBuf, 80, fp);
				}
		fclose(fp);

		//printf("Sou interno e vou mandar essa mensagem para o externo %s\n", str);
		cria_cliente(vetorPortas[0], readBuf);
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
	no = f->inicio;
	
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
	    
	    if (no->info->msg == VOP){
	      lixo = retira(f);
	      sem++;
	    }
	    else if (no->info->msg == POP){
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
		
		//printf("Tratando cliente %s\n", inet_ntoa(echoClntAddrExt.sin_addr));
		recv(clntSockExt, echoBuffer, sizeof(echoBuffer), 0);
				
		sscanf(echoBuffer,"%d %d %d",&idf,&msg,&rel);
		
		//printf("Sou externo e fiz o sscanf %d %d %d\n", idf, msg, rel);

// 		printf("Sou externo e recebi essa msg do interno %s\n",echoBuffer);
		mens_ext = criarInfo(idf, msg, rel);
		sleep(1);
		
 		//printf("Sou externo e criei o info %d %d %d\n",mens_ext->idf, mens_ext->msg, mens_ext->rel);
		
		//atualiza o relogio
		relogio++;
		rel = max(relogio, rel);
		mens_ext->rel = rel;
		
		//switch de tipo de msg
		if((mens_ext->msg == POP) || (mens_ext->msg == VOP)) {
			
			//Insere o nodo na fila de requisicoes
			insere(q_ext,mens_ext);
// 			printf("Sou externo e vou inserir essa mensagem na fila %d %s %d\n", mens_ext->idf, mens_ext->msg, mens_ext->rel);
			
			//Broadcast para todos os outros
			sprintf(echoBuffer,"");
			sprintf(echoBuffer,"%d %d %d",idf,ACK,rel);
// 			printf("Sou externo e vou fazer broadcast dessa mensagem %s\n",echoBuffer);
			
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
			printf("----fila----\n");
			imprimeFila(q_ext);
			printf("----fila----\n");
			
		}
		else if(mens_ext->msg == REQP) {
			
			//Insere o nodo na fila de requisicoes
			mens_ext->msg = POP;
			insere(q_ext,mens_ext);
			//printf("Sou externo e vou inserir essa mensagem na fila %d %d %d\n", mens_ext->idf, mens_ext->msg, mens_ext->rel);
			
			//Broadcast para todos os outros
			sprintf(echoBuffer,"");
			sprintf(echoBuffer,"%d %d %d",idf,POP,rel);
// 			printf("Sou externo e vou fazer broadcast dessa mensagem %s\n",echoBuffer);
			
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
			printf("----fila----\n");
			imprimeFila(q_ext);
			printf("----fila----\n");
		     
		     }
		     else if(mens_ext->msg == REQV){
			    
			    
		            //Insere o nodo na fila de requisicoes
			    mens_ext->msg = VOP;
			    insere(q_ext,mens_ext);
			    //printf("Sou externo e vou inserir essa mensagem na fila %d %d %d\n", mens_ext->idf, mens_ext->msg, mens_ext->rel);
			    
			    //Broadcast para todos os outros
			    sprintf(echoBuffer,"");
			    sprintf(echoBuffer,"%d %d %d",idf,VOP,rel);
// 			    printf("Sou externo e vou fazer broadcast dessa mensagem %s\n",echoBuffer);
			    
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
			    printf("----fila----\n");
			    imprimeFila(q_ext);
			    printf("----fila----\n");
			  }
			  else 
			  {
			      //limpaFila(q_ext);
			      //printf("Sou externo Recebi %s\n", echoBuffer);
			      
			  };
		
	}	
}
