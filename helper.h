#define NUM_PROCS 1 //numero de filosofos

//Variaveis do Socket Interno
extern int servSockInterno, acceptSockInterno, len, t;
extern struct sockaddr_un local, remote;
extern char str[100];

//Variaveis do Socket Externo
extern void HandleTCPClient(int clntSockExt);
extern int servSockExt;
extern int clntSockExt;
extern struct sockaddr_in echoServAddrExt;
extern struct sockaddr_in echoClntAddrExt;
extern unsigned int clntLen;
extern char echoBuffer[100];
extern int numeroPortaExterna;

//Variaveis comuns
extern int idf, rel;
extern char msg[255];
