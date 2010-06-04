#define NUM_PROCS 2 //numero de filosofos
#define ACK 0
#define REQP 1
#define REQV 2
#define POP 3
#define VOP 4

#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
	#define min( a, b ) ( ((a) > (b)) ? (b) : (a) )
#endif

//Variaveis do Socket Interno
extern int servSockInterno, acceptSockInterno, len, t;
extern struct sockaddr_un local, remote;

//Variaveis do Socket Externo
extern void HandleTCPClient(int clntSockExt);
extern int servSockExt;
extern int clntSockExt;
extern struct sockaddr_in echoServAddrExt;
extern struct sockaddr_in echoClntAddrExt;
extern unsigned int clntLen;
extern int numeroPortaExterna;

//Variaveis comuns
extern int ident;
extern int garfo, idf, rel, msg;
extern int meu_idf;
