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
	nodo *n;
	nodo *buf;

	ini = malloc(sizeof (nodo));
	n = malloc(sizeof (nodo));

	ini->info = info;
	ini->prox = NULL;

	n = f->inicio;

	if (f->inicio == NULL) f->inicio = ini;
	else {
		if(((ini->info->rel < f->inicio->info->rel) || ((ini->info->rel == f->inicio->info->rel) && (ini->info->idf < f->inicio->info->idf)))) {
			ini->prox = f->inicio;
			f->inicio = ini;
		}
		else {
			while(((n->prox->info->rel < ini->info->rel) || ((n->prox->info->rel == ini->info->rel) && (n->prox->info->idf < ini->info->idf)))) {
				n = n->prox;
			}
			buf = n->prox;
			n->prox = ini;
			ini->prox = buf;
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

	inf *n;
	nodo *q;

	n = malloc(sizeof (inf));
	q = malloc(sizeof (nodo));

	q = f->inicio;
	do
	{
		n = q->info;
		printf("%d ",n->idf);
		printf("%s ",n->msg);
		printf("%d\n",n->rel);
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

