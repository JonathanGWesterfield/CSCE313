# makefile

all: dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	g++ -std=c++11 -c -g reqchannel.cpp

semaphore.o: Semaphore.h Semaphore.cpp
	g++ -std=c++11 -c -g Semaphore.cpp

boundedbuffer.o: semaphore.o BoundedBuffer.h BoundedBuffer.cpp
	g++ -std=c++11 -c -g BoundedBuffer.cpp

filehandler.o: FileHandler.h FileHandler.cpp
	g++ -std=c++11 -c -g FileHandler.cpp

dataserver: dataserver.cpp reqchannel.o 
	g++ -std=c++11 -g -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp reqchannel.o boundedbuffer.o filehandler.o
	g++ -std=c++11 -g -o client client.cpp reqchannel.o boundedbuffer.o semaphore.o filehandler.o -lpthread

clean:
	rm *.o dataserver client JoeFile.xls JaneFile.xls JohnFile.xls