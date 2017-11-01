#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <time.h>
#include <sys/types.h>

#include <stdlib.h> 
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>

int main(){
	printf("Hello World\n");	
	
	// Method 1
	//Open communication with arduino	
	int portName = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
	
	while(1) {
		char *data = malloc(sizeof(char*));
		strcpy(data,"");
		if(read(portName, data, sizeof(data)) == 0){
			continue;
		}
		usleep(10000);	//keep 10000 as any faster, and the program reads in each character written to the serial connection
		printf("%s\n",data);
		//if (strncmp(data,"12",2) == 0){
		//	break;
		//}
		free(data);
	}
	
	printf("Goodbye World\n");
	
	//Close communication
	close(portName);

	return 0;
}
