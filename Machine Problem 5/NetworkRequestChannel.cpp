/*
    File: NetworkRequestChannel.cpp

    Author: Ya boi its me. Jabroni.
            Department of Computer Science
            Texas A&M University
    Date  : Does it matter?

    NetworkChannel program for MP5 in CSCE 313
*/

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <string.h>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

#include "NetworkRequestChannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* FUNCTION P R O T O T Y P E S FOR LOCAL FUNCTIONS*/
/*--------------------------------------------------------------------------*/
void errorMessage(char* msg);
int createClientConnection(const char* host, const char* portNo);
int createServerConnection(const char* svc, int backLog);

/*--------------------------------------------------------------------------*/
/* DATA TYPES */
/*--------------------------------------------------------------------------*/

struct sockaddr_in serverIn;

/**
 * @brief constructor for the networkRequestChannel. Talks to the client
 * @param _server_host_name
 * @param _port_no
 */
NetworkRequestChannel::NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no)
{
	stringstream ss;
	ss << _port_no;
	string port = ss.str();

	fd = createClientConnection(_server_host_name.c_str(), port.c_str());

}

/**
 * @brief constructor for the networkRequestChannel. Talks to the server
 * @param _port_no
 * @param connection_handler
 * @param backLog
 */
NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port_no, void * (*connection_handler) (void *), int backLog)
{

	stringstream ss;
	ss << _port_no;
	string port = ss.str();

	int master = createServerConnection(port.c_str(), backLog);
	int serverSize = sizeof(serverIn);


	while(true)
	{
		int * slave = new int;

		pthread_t thread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);


		*slave = accept(master,(struct sockaddr*)&serverIn, (socklen_t*)&serverSize);

		if(slave < 0)
		{
			delete slave;

			if(errno == EINTR)
                continue;//retry
			else
                errorMessage((char*)"Unknown error in accept()!!!!!!!");
		}

		pthread_create(&thread, &attr, connection_handler, (void*)slave);


	}
    printf("Connection Complete\n");
}

/**
 * @brief Destructor
 */
NetworkRequestChannel::~NetworkRequestChannel()
{
	close(fd); // Closes the socket
}

/*--------------------------------------------------------------------------*/
/* NetworkRequestChannel Functions */
/*--------------------------------------------------------------------------*/
const int MAX_MSG = 255;

/**
 * @brief returns the file descriptor for any function that needs it
 * @return
 */
int NetworkRequestChannel::readFileDesc()
{
	return fd;
}

/**
 * @brief mechanism for the read communication between processes
 * @return
 */
string NetworkRequestChannel::cRead()
{
	char buf[MAX_MSG];

	if (read(fd, buf, MAX_MSG) < 0)
        errorMessage((char*)"Error reading");

	string s = buf;

	return s;
}

/**
 * @brief mechanism for the write communication between processes
 * @param _msg
 * @return
 */
int NetworkRequestChannel::cWrite(string _msg)
{
	if (_msg.length() >= MAX_MSG)
    {
        errorMessage((char*)"Message too long for Channel");
		return -1;
	}

	const char * s = _msg.c_str();

	if (write(fd, s, strlen(s) + 1) < 0)
        errorMessage((char*)"Error writing!!!");
}

/*--------------------------------------------------------------------------*/
/* NetworkRequestChannel Local Functions */
/*--------------------------------------------------------------------------*/

/**
 * @brief Generic errorMessage message function
 * @param msg
 */
void errorMessage(char* msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/**
 * @brief creates the connection to the client through the network. Also checks to make sure that
 * there are no errors in creating the connection
 * @param host
 * @param portNo
 * @return
 */
int createClientConnection(const char* host, const char* portNo)
{
    struct sockaddr_in sockIn;
    memset(&sockIn, 0, sizeof(sockIn));
    sockIn.sin_family = AF_INET;

    if(struct servent * pse = getservbyname(portNo, "tcp")) // create the port
        sockIn.sin_port = pse->s_port;

    else if ((sockIn.sin_port = htons((unsigned short)atoi(portNo))) == 0)
        errorMessage((char*)"Can't connect to the port");

    if(struct hostent* hn = gethostbyname(host))
        memcpy(&sockIn.sin_addr, hn->h_addr, hn->h_length);

    else if((sockIn.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
    {
        printf("Can't determine the host <%s>\n", host);
        errorMessage((char*)"Can't determine the host");
    }

    int netSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(netSocket < 0)
        errorMessage((char*)"Can't create socket");

    if(connect(netSocket, (struct sockaddr *)&sockIn, sizeof(sockIn)) < 0)
    {
        printf("Can't connect to %s: %s", host, portNo);
        errorMessage((char*)"Can't connect to the host");
    }

    return netSocket;
}

/**
 * @brief Creates a network connection with the server so the client and the server
 * can communicate with each other
 * @param svc
 * @param backLog
 * @return
 */
int createServerConnection(const char* svc, int backLog)
{
    memset(&serverIn, 0, sizeof(serverIn));
    serverIn.sin_family = AF_INET;
    serverIn.sin_addr.s_addr = INADDR_ANY;

    if(struct servent* pse = getservbyname(svc, "tcp"))
        serverIn.sin_port = pse->s_port;

    else if((serverIn.sin_port = htons((unsigned short)atoi(svc))) == 0)
        errorMessage((char*)"Can't get port");

    int socketNum  = socket(AF_INET, SOCK_STREAM, 0);

    if(socketNum < 0)
        errorMessage((char*)"Can't create socket");

    if(bind(socketNum, (struct sockaddr*)&serverIn, sizeof(serverIn)) < 0)
        errorMessage((char*)"CAN'T BIND!!!");

    listen(socketNum, backLog); // check for any connection activity

    return socketNum;
}

