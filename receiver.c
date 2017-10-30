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

typedef struct{
	char dgram[512];
	struct sockaddr_in adr;
	int x;
	int s;
}Data;

static void bail(const char *on_what);
void interperateMessage(Data data);
void getMessage(Data data);

static void bail (const char *on_what) {
	fputs(strerror(errno),stderr);
	fputs("L ",stderr);
	fputs(on_what,stderr);
	fputc('\n',stderr);
	exit(1);
}

void getMessage(Data data){
	y=0;
	while (1){
		y = recvfrom(data.s, 
					data.dgram, 
					sizeof(data.dgram), 
					0, 
					(struct sockaddr *) &(data.adr), 
					&(data.x));
		if (data.x < 0){
			bail("recvfrom()");
		}
		if (y >= 0){
			printf("connection made: y=%d\n",y);
			//message received
			sleep(1);	//sleep for one sec so that the other threads can notice the flag change
			y=0;
		}
		y=0;
	}
}

void interperateMessage(Data data){
	if (y != 0){
		if (strncmp(data.dgram, 'k', 1) == 0){
			fwrite(data.dgram,y,1,stdout);
			char *input = malloc(sizeof(char*)*2);
			strcpy(input,data.dgram);
			if (strncmp(input,"kyle",1) == 0) {
				int status = system("omxplayer blink.mp3");
			}
			putchar('\n');
			fflush(stdout);
		}
	}
}

int main(int argc, char **argv) {
	Data data;
	int z;
	int x;
	struct sockaddr_in adr;
	int len_inet;
	int s;
	char dgram[512];
	static int so_reuseaddr = TRUE;
	static char *bc_addr = "127.255.255.255:9097";
	pthread_t t1,t2;

	if (argc > 1){
		//broadcast address
		bc_addr = argv[1];
	}
	s = socket(AF_INET,SOCK_DGRAM,0);
	data.s = s;

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

	for (;;){ 
		strcpy(data.dgram,dgram);

		data.x = x;
		data.adr = adr;
		pthread_create(&t1,NULL,(void*)getMessage,&data);
		//wait for broadcast message:
		// y = recvfrom(s,	//socket
		// 	dgram,	//receiving buffer
		// 	sizeof(dgram), //max receiving buffer size
		// 	0,	//flags: no options
		// 	(struct sockaddr *)&adr,  //addr
		// 	&x);	//addr len, in & out
		// if (x<0)
		// 	bail("recfrom(2)");

		// fwrite(data.dgram,y,1,stdout);
		// char *input = malloc(sizeof(char*)*2);
		// //input = strdup(dgram);
		// strcpy(input,data.dgram);
		// if (strncmp(input,"kyle",1) == 0) {
		// 	int status = system("omxplayer blink.mp3");
		// }
		// putchar('\n');

		// fflush(stdout);
	}

	return 0;
}





