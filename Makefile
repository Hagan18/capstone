all: receiver

receiver: receiver.o mkaddr.o 
	gcc -g receiver.o mkaddr.o -o receiver

receiver.o: receiver.c
	gcc -c receiver.c -lpthread

mkaddr.o: mkaddr.c
	gcc -c -D_GNU_SOURCE -Wreturn-type -g mkaddr.c

clean:
	rm -rf *.o receiver
