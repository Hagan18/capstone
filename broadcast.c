/********************
 * Kyle Hagan
 * Example call: ./broadcast 10.81.55.255:9097 '10.81.55.116:*'
 * the previous command is made up of the server's broadcast IP
 * the second part is the server's IP with * as the port number
 * so that it will work with all ports
 */

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
#include <pthread.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

long population = 0;
short x;
double IO;
double I;
int z;
int s;
struct sockaddr_in adr_bc;
int len_bc;

extern int mkaddr(void *addr,
		  int *addrlen,
		  char *str_addr,
		  char *protocol);

static void bail (const char *on_what){
	fputs(strerror(errno),stderr);
	fputs(": ",stderr);
	fputs(on_what,stderr);
	fputc('\n',stderr);
	exit(1);
}

void monitorPopulation(void){
	
	int portName = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);

	while(1){
		char *data = malloc(sizeof(char*));
		strcpy(data,"");	//make sure it's empty
		if (read(portName, data, sizeof(data)) == 0){
			continue;
		}
		usleep(10000); //don't decrease as it will start reading in 1 char at a time
		long tempPopulation = strtol(data,NULL,10);
		
		//printf("population data: %ld\n",population);

		if (population < 20 && tempPopulation >= 20){
			z = sendto(s, "+", 1, 0, (struct sockaddr *)&adr_bc, len_bc);
			if (z == -1){
				bail("sendto()");
			}
			printf("increasing volume\n");
		}
		else if (population >= 20 && tempPopulation <= 19){
			z = sendto(s, "-",1,0,(struct sockaddr *)&adr_bc,len_bc);
			if (z == -1){
				bail("sendto()");
			}
			printf("decreasing volume\n");
		}
		population = tempPopulation;
		
		free(data);
	}
	close(portName); //never actually reaches here
}

int main(int argc, char**argv){
	char bcbuf[512], *bp;
	struct sockaddr_in adr_srvr;
	int len_srvr;
	static int so_broadcast = TRUE;
	static char *sv_addr, *bc_addr; 
	pthread_t t1;

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

	pthread_create(&t1,NULL,(void*)monitorPopulation,NULL);
	
	printf("Please choose a command from the following list:\n");
	printf("----------------------------------\n"
		"play: play the playlist.\n"
		"+   : manually increase volume\n"
		"-   : manually decrease volume\n"
		"q   : skip the current the audio playback\n"
		"----------------------------------\n");

	while(1) {
		bp = bcbuf;
		char input[256];
		int i;
		printf("Enter a command: ");
		if (fgets(input, sizeof(input),stdin)){
			if(sscanf(input,"%d", &i)){}
		}
		z = sendto(s, input, strlen(input), 0, (struct sockaddr *)&adr_bc, len_bc);
		if(z==-1)
			bail("sendto()");
	}
	return 0;
}



