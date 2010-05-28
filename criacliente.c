#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "criacliente.h"

void cria_cliente(int numeroPorta, char *mensagem) {

    int sock;
    char teste[255];
    struct sockaddr_in echoServAddr;
    char enderecoIPServidor[100];

    printf("vou criar cliente pra %d com msg %s\n",numeroPorta, mensagem);

    strcpy (enderecoIPServidor, "127.0.0.1");

    /* Criacao do Socket TCP */
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* construcao do endereco de conexao */
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(enderecoIPServidor);
    echoServAddr.sin_port = htons(numeroPorta);

    /* Conectando ao servidor */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0){
			perror ("connect() socket cliente do interno falhou:");
			//exit(1);
    }

    /* Interacao com o servidor */
    int i;
    send(sock, mensagem, sizeof(teste), 0);
    close(sock);
//     printf("mandei coisinha pro externo %d %s\n",numeroPorta, mensagem);
}
