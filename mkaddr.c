#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int mkaddr(void *addr, int *addrlen, char *str_addr, char *protocol) {
	char *inp_addr = strdup(str_addr);
	char *host_part = strtok(inp_addr, ":");
	char *port_part = strtok(NULL, "\n");
	struct sockaddr_in *ap = (struct sockaddr_in *) addr;
	struct hostent *hp = NULL;
	struct servent *sp = NULL;
	char *cp;
	long lv;
	
	if (!host_part)
		host_part = "*";
	if (!port_part)
		port_part = "*";
	if (!protocol)
		protocol = "tcp";

	memset(ap,0,*addrlen);
	ap->sin_family = AF_INET;
	ap->sin_port = 0;
	ap->sin_addr.s_addr = INADDR_ANY;

	if (strcmp(host_part,"*") == 0)
		;	//leave as INADDR_ANY
	else if (isdigit(*host_part)){
		//ap->sin_addr.s_addr = inet_addr(host_part);
		//if (ap->sin_addr.s_addr == INADDR_NONE)
		if (!inet_aton(host_part,&ap->sin_addr))
			return -1;
	}
	else {
		//assume hostname
		hp = gethostbyname(host_part);
		if (!hp)
			return -1;
		if (hp->h_addrtype != AF_INET)
			return -1;
		ap->sin_addr = *(struct in_addr *) hp->h_addr_list[0];
	}

	if (!strcmp(port_part,"*"))
		;	//leave as wild (zero)
	else if (isdigit(*port_part)){
		lv = strtol(port_part,&cp,10);
		if (cp != NULL && *cp)
			return -2;
		if (lv < 0L ||lv >= 32768)
			return -2;
		ap->sin_port = htons((short)lv);
	}
	else {
		//lookup the service;
		sp = getservbyname(port_part,protocol);
		if (!sp)
			return -2;
		ap->sin_port = (short) sp->s_port;
	}

	*addrlen = sizeof(*ap);
	free(inp_addr);
	return 0;
}



