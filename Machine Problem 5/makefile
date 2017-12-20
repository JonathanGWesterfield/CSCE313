# makefile

all: dataserver client

boundedbuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -std=c++11 -c -g BoundedBuffer.cpp

filehandler.o: FileHandler.h FileHandler.cpp
	g++ -std=c++11 -c -g FileHandler.cpp

NetworkRequestChannel.o: NetworkRequestChannel.h NetworkRequestChannel.cpp
	g++ -c -g NetworkRequestChannel.cpp

dataserver: dataserver.cpp NetworkRequestChannel.o
	g++ -g -o dataserver dataserver.cpp NetworkRequestChannel.o

client: client.cpp NetworkRequestChannel.o boundedbuffer.o filehandler.o
	g++ -g -o client client.cpp NetworkRequestChannel.o boundedbuffer.o filehandler.o

clean:
	rm *.o dataserver client JoeFile.xls JaneFile.xls JohnFile.xls
