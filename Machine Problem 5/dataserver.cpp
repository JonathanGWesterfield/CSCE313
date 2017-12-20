/*
    File: dataserver.cpp

    Author: Ya boi its me. Jabroni.
            Department of Computer Science
            Texas A&M University
    Date  : Does it matter?

    Dataserver program for MP5 in CSCE 313
*/

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <string.h>
#include <sstream>
#include "NetworkRequestChannel.h"

using namespace std;

int MAX_MSG = 255;

/*--------------------------------------------------------------------------*/
/* FUNCTION P R O T O T Y P E S FOR LOCAL FUNCTIONS*/
/*--------------------------------------------------------------------------*/
void error(char *msg);
string int2string(int n);
string serverRead(int * fileDesc);
int serverWrite(int * fileDesc, string m);
void processHello(int * fileDesc, const string & _request);
void processData(int * fileDesc, const string &  _request);
void processRequest(int * fileDesc, const string & _request);
void *connection_handler(void * arg);


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

/**
 * @brief The server. The server communicates using the networkRequestChannel and
 * responds to specific requests accordingly
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char * argv[])
{
    int backLog = 100;
    unsigned short portNum = 10250;
    unsigned short pN = 0;
    int bLog = 0;

    int c = 0;
    while((c = getopt(argc, argv, "p:b:")) != -1)
    {
        switch(c)
        {
            case 'p':
                pN = atoi(optarg);
                break;
            case 'b':
                bLog = atoi(optarg);
                break;
            case '?':
                printf("Unknown option\nAborting\n\n");
                abort();
            default:
                printf("cmd line error\n");
                printf("options -b=backlogServerSocket -p=portNumberForDataServer\n\n");
                abort();
        }
    }

    if(pN != 0)
        portNum = pN;

    if(bLog != 0)
        backLog = bLog;

    printf("SERVER HAS STARTED ON PORT: %d\n", portNum);
    printf("Backlog size is: %d\n", backLog);

    NetworkRequestChannel server(portNum, connection_handler, backLog);

    server.~NetworkRequestChannel();

    return 0;
}

/*--------------------------------------------------------------------------*/
/* FUNCTIONS */
/*--------------------------------------------------------------------------*/

/**
 * @brief generic error message
 * @param msg
 */
void error(char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/**
 * @brief copied from server in previous machine problems
 * @param n
 * @return
 */
string int2string(int n)
{
    stringstream ss;
    ss << n;
    return ss.str();
}

/**
 * @brief The mechanism to be able to write what is happening across different processes
 * @param fileDesc
 * @return
 */
string serverRead(int * fileDesc)
{
    char buf[MAX_MSG];

    read(*fileDesc, buf, MAX_MSG);
    string s = buf;
    return s;
}

/**
 * @brief The mechanism to be able to write what is happening across different processes
 * @param fileDesc
 * @param m
 * @return
 */
int serverWrite(int * fileDesc, string m)
{
    if(m.length() >= MAX_MSG)
        error((char*)"Message is too big to fit\n");

    if(write(*fileDesc, m.c_str(), m.length()+1) < 0)
        error((char*)"Write Error\n");
}

/**
 * @brief Confirms that the connection was made by saying hello back.
 * Just in its own special way.
 * @param fileDesc
 * @param _request
 */
void processHello(int * fileDesc, const string & _request)
{
    serverWrite(fileDesc, "WASSUP JABRONI!!!");
    return;
}

/**
 * @brief Generates a random number to send back to the client
 * @param fileDesc
 * @param _request
 */
void processData(int * fileDesc, const string &  _request)
{
    usleep(1000 + (rand() % 5000));

    serverWrite(fileDesc, int2string(rand() % 100));
    return;
}

/**
 * @brief Decides what to do depending on if the requests is a request
 * for data or if it is making the initial handshake and confirming the
 * connection
 * @param fileDesc
 * @param _request
 */
void processRequest(int * fileDesc, const string & _request)
{
    if (_request.compare(0, 5, "hello") == 0)
        processHello(fileDesc, _request);

    else if (_request.compare(0, 4, "data") == 0)
        processData(fileDesc, _request);
    return;
}

/**
 * @brief Creates a new connection and keeps track if the user decides to terminate the connection
 * @param arg
 * @return
 */
void *connection_handler(void * arg)
{
    int * fileDesc = (int*)arg;

    if(fileDesc == NULL)
        error((char*)"ERROR!!! File descriptor is NULL\n");

    printf("New Connection\n");

    while(1)
    {
        string request = serverRead(fileDesc);

        if (request.compare("quit") == 0)
        {
            serverWrite(fileDesc, "Didn't want to talk to you anyway!");
            usleep(8000);
            break;
        }

        processRequest(fileDesc, request);
    }
    printf("Connection Closed\n");
}



