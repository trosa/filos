#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/stat.h>
#include <linux/stat.h>

#include "helper.h" //Num procs + MAX

#define FIFO_FILE "MYFIFO"
#define GO_FILE "GOFIFO"

int relogio = 0;
int cliSockInterno, t, len;
struct sockaddr_un remote;
char msgBuf[100];

void comer(int idf)
{
	printf("(%d, %d) EATING\n",idf,relogio);
	relogio++;
	srand(time(NULL));
	int num = (rand()%5)+1; 
	sleep(num); 
}

void pensar(int idf)
{
	printf("(%d, %d) THINKING\n",idf,relogio);
	relogio++;
	srand(time(NULL));
	sleep((rand()%5)+1); 
}

void pedir(int idf, int garfo, int eh_vop)
{
	

	int mess;
	relogio++;
	
	FILE *fp;
	
	if ((fp = fopen(FIFO_FILE,"w")) == NULL) {
		perror("fopen nao deu");
	}
	
	if(eh_vop) {
		sprintf(msgBuf,"");
		sprintf(msgBuf, "%d %d %d %d",garfo,idf,REQV,relogio);
		fputs(msgBuf, fp);
		fclose(fp);
		return;
	}

	sprintf(msgBuf,"");
	sprintf(msgBuf, "%d %d %d %d",garfo,idf,REQP,relogio);
	fputs(msgBuf, fp);

	fclose(fp);

	//FIFO DE ESPERA
	FILE *fg;
	
	char goBuf[100]; char goTesteBuf[100];

	umask(0);
	mknod(GO_FILE, S_IFIFO|0666, 0);

	strcpy(goBuf, goTesteBuf);
	fg = fopen(GO_FILE, "r");
	fgets(goBuf, 80, fg);
	while(!strcmp(goTesteBuf,goBuf)) {
			fclose(fg);
			fg = fopen(GO_FILE, "r");
			fgets(goBuf, 80, fg);
	}
	fclose(fg);

	int rel_temp;
	sscanf(goBuf,"%d",&rel_temp);
	relogio = max(rel_temp,relogio);

	sleep(1);
	
}

void *filosofo(int *num_idf)
{
	int idf = (int *)num_idf;
	
	//int right = (idf+NUM_PROCS-1)%NUM_PROCS;
	//int left = (idf+1)%NUM_PROCS;
	int right = 0;
	int left = 1;
	
	while(1)
	{
		printf("(%d, %d) HUNGRY\n",idf,relogio);
		if (idf != NUM_PROCS) {
			pedir(idf,left,0);
			pedir(idf,right,0);
		}
		else {
			pedir(idf,right,0);
			pedir(idf,left,0);
		}
    comer(idf);
		pedir(idf,left,1);
		sleep(2);
		pedir(idf,right,1);
		pensar(idf);
  }
}

int main(int argc, char *argv[])
{  
	int i; 
	i = atoi(argv[1]);
	
  	pthread_t fil;
   
        pthread_create(&fil, NULL, (void *) filosofo, (int *) i);
  
  	pthread_join(fil, NULL);
  
  	return 0;
    
}
  
