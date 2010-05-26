#include <stdlib.h>
#include <stdio.h>
#include "fila.h"

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


// Cria uma fila vazia e devolve o endereço da nova fila.

fila *criaFila( void) {
	fila *f;
	f = malloc( sizeof (fila));
	if (f == NULL) return NULL;
	f->inicio = NULL;
	return f;
}

// Libera o espaço ocupado pela fila q. A função supõe que
// o espaço apontado pelo campo element de cada célula da
// lista tenha sido alocado por malloc.

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
