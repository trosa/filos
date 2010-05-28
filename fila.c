#include <stdlib.h>
#include <stdio.h>

typedef struct inf{
	int idf;
	char *msg;
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

inf *criarInfo(int idf, char *msg, int rel)
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
  	nodo *ini;
	ini = malloc(sizeof (nodo));
	ini->info = info;
	ini->prox = NULL;
	if (f->inicio == NULL) f->inicio = ini;
	else f->fim->prox = ini;
	f->fim = ini;
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
		printf("%s ",q->info->msg);
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

