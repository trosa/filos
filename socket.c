#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "socket.h"
  
int sock;
struct sockaddr_in echoServAddr;
char echoBuffer[9];
int bytesRcvd, totalBytesRcvd;
 
char enderecoIPServidor[100];
int numeroPorta;
int tamanhoRecebido;

/**********************************************************************/
void cria_log_socket(char ip, int porta)
{    
  // Checagem de parametros
  strcpy (enderecoIPServidor, ip);
  numeroPorta = porta;
  
  //Criação do socket
  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);   
}
  
void cria_conexao_logica()
{
  //Construindo a conexão
  memset(&echoServAddr, 0, sizeof(echoServAddr));
  echoServAddr.sin_family = AF_INET;
  echoServAddr.sin_addr.s_addr = inet_addr(enderecoIPServidor);
  echoServAddr.sin_port = htons(numeroPorta);
  
  //Conectando ao servidor
  if (connect(sock, (struct sockaddr *) &echoServAddr,
		    sizeof(echoServAddr)) < 0){
    perror ("connect() falhou:");
    exit(1);
  }
  
}

void send_helper(int idf, char msg, int relogio)
{
  send(sock, msg, sizeof(msg), 0);  
}

/**********************************************************************/