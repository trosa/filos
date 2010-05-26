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

//Costrutores
inf *criarInfo(int idf, char *msg, int rel);
fila *criaFila(void);

//Ferramentas de Manipulacao da fila
void insere(fila *f, inf *info);
inf *retira(fila *f);
void destroiFila(fila *f);
