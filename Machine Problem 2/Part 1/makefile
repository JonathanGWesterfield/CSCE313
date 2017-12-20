# makefile

all: dataserver client

reqchannel.o: reqchannel.H reqchannel.C
	g++ -c -g reqchannel.C

dataserver: dataserver.C reqchannel.o 
	g++ -o dataserver dataserver.C reqchannel.o -lpthread

client: client.c reqchannel.o
	g++ -o client client.c reqchannel.o

clean:
	rm *.o dataserver client
