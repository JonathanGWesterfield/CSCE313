/*
    File: NetworkRequestChannel.cpp

    Author: Ya boi its me. Jabroni.
            Department of Computer Science
            Texas A&M University
    Date  : Does it matter?

    NetworkChannel program for MP5 in CSCE 313
*/

#ifndef _NETWORKREQUESTCHANNEL_H_
#define _NETWORKREQUESTCHANNEL_H_


#include <iostream>
#include <fstream>
#include <string>

#include "semaphore.h"


using namespace std;

class NetworkRequestChannel {

private:

  int fd; // socket file descriptor;

public:

  NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no);
  NetworkRequestChannel(const unsigned short _port_no, void * (*connection_handler) (void *), int backlog);

  ~NetworkRequestChannel();

  string cRead();

  int cWrite(string _msg);
  int readFileDesc();

};


#endif


