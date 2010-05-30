#include <stdlib.h>
#include <stdio.h>

#include "helper.h"

typedef struct inf{
	int idf;
	int msg;
	int rel;
} inf;

typedef struct nodo{
	inf *info;
	struct nodo *prox;
} nodo;

typedef struct{
	nodo *inicio;
	nodo *fim;
} fila;

inf *criarInfo(int idf, int msg, int rel)
{
	inf *info;
	info = malloc(sizeof(info));
	info->idf = idf;
	info->msg = msg;
	info->rel = rel;
	return info;
}

void insere(fila *f, inf *info)
{

	nodo *ins, *buf, *n;
	ins = malloc(sizeof (nodo));
	ins->info = info;
	ins->prox = NULL;
	if (f->inicio == NULL) f->inicio = ins;
	else {
		if (f->inicio->prox == NULL) { //se so tiver 1 elemento
			if ((ins->info->rel < f->inicio->info->rel) || ((ins->info->rel == f->inicio->info->rel) && (ins->info->idf <= f->inicio->info->idf))){
				//se o rel do ins for menor que o do elemento
				buf = f->inicio;
				f->inicio = ins; //coloca ins no comeco da fila
				ins->prox = buf;
			}
			else { //se o rel do ins for maior que o do elemento
				f->inicio->prox = ins;  //coloca ins depois do n
				ins->prox = NULL;
			}
		}
		else { //se tiver mais de 1 elemento
			if ((ins->info->rel < f->inicio->info->rel) || ((ins->info->rel == f->inicio->info->rel) && (ins->info->idf <= f->inicio->info->idf))) {
				//se o rel do ins for menor que o do primeiro da fila
				buf = f->inicio;
				f->inicio = ins; //coloca o ins no come;o da fila
				ins->prox = buf;
			}
			else {
				n = f->inicio;
				while ((n->prox != NULL) && ((ins->info->rel > n->prox->info->rel) || ((ins->info->rel == n->prox->info->rel) && (ins->info->idf > n->prox->info->idf)))) { //enqto nao chegar no fim e rel do ins for menor que do proximo
					n = n->prox; //vai percorrendo
				} //qdo parar quer dizer que o rel do n eh o ultimo que eh menor que o de ins, o proximo vai ser maior, entao insiro depois de n
				buf = n->prox;
				n->prox = ins;
				ins->prox = buf;
			}
		}
	}
}

inf *retira(fila *f)
{
	inf *result;
	nodo *ini;
	ini = f->inicio;
	if (ini == NULL)
	{
		printf("Erro: a fila esta vazia");
		return NULL;
	} 
	result = ini->info;
	f->inicio = ini->prox;
	free(ini);
	return result;
}

fila *criaFila( void) {
	fila *f;
	f = malloc( sizeof (fila));
	if (f == NULL) return NULL;
	f->inicio = NULL;
	return f;
}

void imprimeFila(fila *f){
	if (f == NULL)
		printf("Fila Vazia");

// 	inf *n;
	nodo *q;

// 	n = malloc(sizeof (inf));
	q = malloc(sizeof (nodo));

	q = f->inicio;
	do
	{
// 		n = q->info;
		printf("%d ",q->info->idf);
		
		if (q->info->msg == 3)
		  printf("POP ");
		else if (q->info->msg == 4)
		  printf("VOP ");
		
		printf("%d\n",q->info->rel);
		q = q->prox;
	} while(q != NULL);
	free(q);
}

void destroiFila( fila *f) {
	nodo *ini, *prox;
	ini = f->inicio;
	while (ini != NULL) {
		prox = ini->prox;
		free(ini->info);
		free(ini);
		ini = prox;
	}
	free(f);
}

