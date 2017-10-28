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

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern int mkaddr(void *addr, int *addrlen, char *str_addr, char *protocol);

static void bail (const char *on_what) {
	fputs(strerror(errno),stderr);
	fputs("L ",stderr);
	fputs(on_what,stderr);
	fputc('\n',stderr);
	exit(1);
}

int main(int argc, char **argv) {
	printf("in main\n");
	int z;
	int x;
	struct sockaddr_in adr;
	int len_inet;
	int s;
	char dgram[512];
	static int so_reuseaddr = TRUE;
	static char *bc_addr = "127.255.255.255:9097";

	if (argc > 1){
		//broadcast address
		bc_addr = argv[1];
	}
	s = socket(AF_INET,SOCK_DGRAM,0);
	if (s == -1)
		bail("socket()");

	//form broadcast address
	len_inet = sizeof(adr);
		
	z = mkaddr(&adr,&len_inet,bc_addr,"udp");
	printf("mkaddr\n");
	if (z==-1)
		bail("bad broadcast address");

	//allow multiple users on the broadcast address
	z = setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&so_reuseaddr,sizeof(so_reuseaddr));
	printf("set options\n");
	if (z == -1)
		bail("setsockopt(SO_REUSEADDR)");

	z = bind(s,(struct sockaddr *)&adr, len_inet);
	printf("bounded\n");
	if (z==-1)
		bail("bind(2)");

	for (;;){
		//wait for broadcast message:
		printf("waiting for message\n");
		z = recvfrom(s,	//socket
			dgram,	//receiving buffer
			sizeof(dgram), //max receiving buffer size
			0,	//flags: no options
			(struct sockaddr *)&adr,  //addr
			&x);	//addr len, in & out
		if (x<0)
			bail("recfrom(2)");

		fwrite(dgram,z,1,stdout);
		
		if (strcmp(dgram,"1") == 0) {
			int status = system("omxplayer blink.mp3");
		}

		putchar('\n');

		fflush(stdout);
	}

	return 0;
}





