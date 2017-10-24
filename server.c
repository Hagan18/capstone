//
//  server.c
//  sercer
//
//  Created by Kyle Hagan on 10/23/17.
//  Copyright © 2017 Kyle Hagan. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/*	reports error and exits back to shell 	*/
static void bail(const char* on_what) {
	if (errno != 0) {
		fputs(strerror(errno),stderr);
		fputs(": ",stderr);
	}
	fputs(on_what,stderr);
	fputc('\n',stderr);
	exit(1);
}

int main(int argc,char **argv) {
	int z;
	char* srvr_addr = NULL;
	char* srvr_port = "9099";
	struct sockaddr_in addr_srvr;	//AF_INET
	struct sockaddr_in adr_clnt;	//AF_INET
	int len_inet;					//lenght
	int s;							//socket
	int c;							//clinet socket
	int n;							//bytes
	time_t td;						//current date&time
	char dtbuf[128];				//date/time info

	/*
	 * Use a server address from the command line, if one was provided.
	 * Otherwise, this program will default to using the arbitrary address 127.0.01
	*/
	if (argc >= 2) {
		/*	address on cmd len_inet	*/
		//strcpy(srvr_addr, argv[1]);
		srvr_addr = strdup(argv[1]);
	}
	else {
		/*	use default address:	*/
		srvr_addr = "127.0.0.1";
	}

	/*	If there's a second argument on the command line, use it as the port #	*/
	if (argc >= 3) {
		//strcpy(srvr_port,argv[2]);
		srvr_port = strdup(argv[2]);
	}

	/*	create a TCP/IP socket to Use 	*/
	s = socket(PF_INET,SOCK_STREAM,0);
	if (s == -1){
		bail("socket()");
	}

	/*	Create a server socket address:	*/
	memset(&addr_srvr,0,sizeof(addr_srvr));
	addr_srvr.sin_family = AF_INET;
	addr_srvr.sin_port = htons(atoi(srvr_port));
	if (strcmp(srvr_addr,"*") != 0){
		/*	Normal address:	*/
		addr_srvr.sin_addr.s_addr = inet_addr(srvr_addr);
		if (addr_srvr.sin_addr.s_addr == INADDR_NONE) {
			bail("bad address");
		}
	}
	else {
		/*	Unexpected address	*/
		addr_srvr.sin_addr.s_addr = INADDR_ANY;
	}

	/*	Bind the address	*/
	len_inet = sizeof(addr_srvr);
	z = bind(s,(struct sockaddr *) &addr_srvr, len_inet);
	if(z == -1){
		bail("bind(2)");
	}

	/*	Make it a listening socket 	*/
	z = listen(s,10);
	if (z == -1){
		bail("listen(2)");
	}

	/*	Start the server loop:	*/
	while(1){
		/* wait for a connection	*/
		len_inet = sizeof(adr_clnt);
		c = accept(s,(struct sockaddr *) &adr_clnt,(socklen_t *) &len_inet);
		if (c == -1) {
			bail("accept(2)");
		}

		/*	generate a time stamp:	*/
		time(&td);
		//n = (int) strftime(dtbuf, sizeof(dtbuf),"%A %b %d %H:%M:%S %Y\n",localtime(&td));
		n = (int) strftime(dtbuf, sizeof(dtbuf),"%A %b %d\n",localtime(&td));
		/*	write back to client: 	*/
		//z = write(c,dtbuf,n);
		z = write(c,"Kyle",6);
		if (z == -1){
			bail("write(2)");
		}

		close(c);
	}
	/*	This portion is never reached because of the infinite loop	*/
	return 0;
}
