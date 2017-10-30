#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern int mkaddr(void *addr, int *addrlen, char *str_addr, char *protocol);
int y;
int x;
int s;
int z;
char dgram[512];
struct sockaddr_in adr;
Display *display;
unsigned int keycode;
int playing=0; //flag used to determine if playing

typedef struct{
	char dgram[512];
	struct sockaddr_in adr;
	int x;
	int s;
}Data;

static void bail(const char *on_what);
void interperateMessage();
void getMessage();

static void bail (const char *on_what) {
	fputs(strerror(errno),stderr);
	fputs("L ",stderr);
	fputs(on_what,stderr);
	fputc('\n',stderr);
	exit(1);
}

void getMessage(void){
	pthread_t t2;
	while (1){
		z = recvfrom(s,	dgram,sizeof(dgram),0,(struct sockaddr *) &adr, &x);
		printf("%d\n",y);
		if (x < 0){
			bail("recvfrom()");
		}
		if (z >= 0){
			printf("connection made: y=%d\n",z);

			//message received
			pthread_create(&t2,NULL,(void*)interperateMessage,NULL);
		}

	}
}

void interperateMessage(){
	if (z >= 0){
		char *command = malloc(sizeof(char*)*50);
		strcpy(command,"omxplayer -o local ");
		char *input = malloc(sizeof(char*)*5);
		strcpy(input,dgram);
		//printf(input);
		fwrite(dgram,y,1,stdout);
		char *song = malloc(sizeof(char*)*5);

		strcpy(song,"blink.mp3");
		strcat(command,song);
		printf("%s\n",command);
		if (strncmp(input,"kyle",1) == 0 && !playing) {
			int status = system(command);
			playing = 1;
		}
		else if(strncmp(input,"pause",1) == 0){
			printf("pause statement\n");
			int status = system("xdotool key p");
		}
		else if(strncmp(input,"+",1) == 0){
			int status = system("xdotool key plus");
		}
		else if(strncmp(input,"-",1) == 0){
			int status = system("xdotool key minus");
		}
		putchar('\n');
		fflush(stdout);
	}
}

int main(int argc, char **argv) {
	int len_inet;
	static int so_reuseaddr = TRUE;
	static char *bc_addr = "127.255.255.255:9097";
	pthread_t t1;

	if (argc > 1){
		//broadcast address
		bc_addr = argv[1];
	}
	s = socket(AF_INET,SOCK_DGRAM,0);
	printf("address: %s\n",bc_addr);

	if (s == -1)
		bail("socket()");

	//form broadcast address
	len_inet = sizeof(adr);
	
	z = mkaddr(&adr,&len_inet,bc_addr,"udp");

	if (z==-1)
		bail("bad broadcast address");

	//allow multiple users on the broadcast address
	z = setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&so_reuseaddr,sizeof(so_reuseaddr));

	if (z == -1)
		bail("setsockopt(SO_REUSEADDR)");

	z = bind(s,(struct sockaddr *)&adr, len_inet);
	printf("bounded\n");
	if (z==-1)
		bail("bind(2)");

	//start thread to receive data
	pthread_create(&t1,NULL,(void*)getMessage,NULL);

	while (1){
		sleep(1000);
	}

	return 0;
}





