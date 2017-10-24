//
//  main.c
//  client
//
//  Created by Kyle Hagan on 10/23/17.
//  Copyright © 2017 Kyle Hagan. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

static void bail(const char *on_what) {
    fputs(strerror(errno),stderr);
    fputs(": ", stderr);
    fputs(on_what,stderr);
    fputc('\n',stderr);
    exit(1);
}

int main(int argc, const char * argv[]) {
    int z;
    char *srvr_addr = NULL;
    char *srvr_port;
    struct sockaddr_in adr_srvr;    //AF_INET
    int len_inet;                   //length
    int s;                          //socket
    struct servent *sp;             //service entry
    char dtbuf[128];                //date/time info
    
    /*
     * Use a server address from the command line, if one has been provided.
     * Otherwise, this program will default to useing the arbitrary addr 
     * 127.0.0.1
     */

    if (argc >= 2) {
        /*  Addr on cmdline: */
        //srvr_addr = argv[1];
        //strcpy(srvr_addr,argv[1]);
    	srvr_addr = strdup(argv[1]);
    }
    else {
        /*  Use default address: */
        srvr_addr = "127.0.0.1";
    }

    if (argc >= 3) {
	    //srvr_port = argv[2];
	    srvr_port = strdup(argv[2]);
    }
    else {
	srvr_port = strdup("9099");
    }

    /*  Create a server socket address  */
    memset(&adr_srvr,0,sizeof(adr_srvr));
    adr_srvr.sin_family = AF_INET;
    adr_srvr.sin_port = htons(atoi(srvr_port));
    adr_srvr.sin_addr.s_addr = inet_addr(srvr_addr);
    
    if (adr_srvr.sin_addr.s_addr == INADDR_NONE) {
        bail("bad address.");
    }
    
    len_inet = sizeof(adr_srvr);
    
    /*  Create a TCP/IP socket to use:  */
    s = socket(PF_INET,SOCK_STREAM,0);
    if (s == -1) {
        bail("socket()");
    }
    
    /*  Connect to the server:  */
    z = connect(s,(struct sockaddr *)&adr_srvr,len_inet);
    if (z == -1) {
        bail("connect(2)");
    }
    
    /*  Read the date/time info:    */
    z = read(s,&dtbuf,sizeof(dtbuf)-1);
    if (z == -1) {
        bail("read(2)");
    }
    
    /*  Report the date and time    */
    dtbuf[z] = 0;   //null terminate string
    
    printf("Date & Time is: %s\n",dtbuf);
    
    /*  Close the socket and exit   */
    close(s);
    putchar('\n');
    
    return 0;
}

