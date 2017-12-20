//
// Created by Jonathan G. Westerfield on 10/24/17.
//

#ifndef CLIONCODE_BOUNDEDBUFFER_H
#define CLIONCODE_BOUNDEDBUFFER_H

#include "semaphore.h"
#include <string>
#include <queue>

class Response
{
public:
    std::string requestData;
    int requestID; // client number for helping worker thread determine which buffer to send to
    int count; // number of times this request has been made

    Response(std::string requestData, int requestID, int requestNumber) // , int requestNumber)
    {
        this->requestData = requestData;
        this->requestID = requestID;
        this->count = requestNumber;
    }
    void setRequestID(int id)
    {
        this->requestID = id;
    }
    void setRequestData(std::string data)
    {
        this->requestData = data;
    }
    int setRequestNumber(int num)
    {
        this->count = num;
    }
};

class BoundedBuffer
{
private:
    Semaphore empty = Semaphore(0);
    Semaphore full = Semaphore(0);
    Semaphore lock = Semaphore(1);
    int BoundedBufferSize;
    std::queue<Response> bufferQ;

public:
    BoundedBuffer(int bbSize); // constructor
    ~BoundedBuffer(); // destructor
    void AddToBuffer(Response response);
    Response RemoveFromBuffer();

};


#endif //CLIONCODE_BOUNDEDBUFFER_H
