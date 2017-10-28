#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern int mkaddr(void *addr,
		  int *addrlen,
		  char *str_addr,
		  char *protocol);
#define MAXQ 4

static struct {
	char *index;
	int start;
	int volit;
	int current;
} quotes[] = {
	{ "DJIA", 1030330,  357 },
	{ "NASDAQ", 276175, 125 },
	{ "S&P 500", 128331, 50 }, 
	{ "TSE 300", 689572, 75 }, 
};

static void initialize (void) {
	short x;
	time_t td;

	time(&td);
	srand((int)td);

	for(x=0; x< MAXQ; ++x){
		quotes[x].current = quotes[x].start;
	}
}

static void gen_quote(void) {
	short x;
	short v;
	short h;
	short r;

	x = rand() % MAXQ;
	v = quotes[x].volit;
	h = (v/2) - 2;
	r = rand() % v;

	if (r<h)
		r = -r;
	
	quotes[x].current += r;
}

static void bail (const char *on_what){
	fputs(strerror(errno),stderr);
	fputs(": ",stderr);
	fputs(on_what,stderr);
	fputc('\n',stderr);
	exit(1);
}

int main(int argc, char**argv){
	short x;
	double IO;
	double I;
	char bcbuf[512], *bp;
	int z;
	int s;
	struct sockaddr_in adr_srvr;
	int len_srvr;
	struct sockaddr_in adr_bc;
	int len_bc;
	static int so_broadcast = TRUE;
	static char *sv_addr = "127.0.0.1:*", *bc_addr = "127.255.255.255:9097";

	if (argc > 2){
		sv_addr = argv[2];
	}
	if ( argc > 1 ) {
		bc_addr = argv[1];
	}

	len_srvr = sizeof(adr_srvr);

	z = mkaddr(&adr_srvr, &len_srvr, sv_addr,"udp");

	if (z==-1){
		bail("bad server address");
	}

	len_bc = sizeof(adr_bc);

	z = mkaddr(&adr_bc,&len_bc, bc_addr,"udp");

	if (z==-1){
		bail("bad broadcast address");
	}

	s = socket(AF_INET,SOCK_DGRAM,0);
	if (s==-1){
		bail("socket()");
	}

	z = setsockopt(s,SOL_SOCKET,SO_BROADCAST,&so_broadcast, sizeof(so_broadcast));

	if(z==-1){
		bail("setsockopt(SO_BROADCAST)");
	}
	
	z = bind(s, (struct sockaddr *)&adr_srvr,len_srvr);

	if (z==-1)
		bail("bind()");

	initialize();

	for (;;) {
		gen_quote();
		printf("in for\n");
		bp = bcbuf;
		for (x=0; x<MAXQ; ++x){
			IO = quotes[x].start / 100.0;
			I = quotes[x].current / 100.0;
			sprintf(bp,"%-7.7s %8.2f %+.2f\n", quotes[x].index, I, I-IO);

			bp += strlen(bp);
		}
		char input[256];
		int i;
		printf("enter some shit: ");
		if (fgets(input, sizeof(input),stdin)){
			if(sscanf(input,"%d", &i)){}
		}
		z = sendto(s, input, strlen(input), 0, (struct sockaddr *)&adr_bc, len_bc);
		printf("sent\n");
		if(z==-1)
			bail("sendto()");

		sleep(4);
	}
	return 0;
}



