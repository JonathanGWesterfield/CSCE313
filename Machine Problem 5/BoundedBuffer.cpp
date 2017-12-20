//
// Created by Jonathan G. Westerfield on 10/24/17.
//

#include "BoundedBuffer.h"

using namespace std;

BoundedBuffer::BoundedBuffer(int bbSize)
{
    this->BoundedBufferSize = bbSize;
    this->empty = Semaphore(BoundedBufferSize);
}

BoundedBuffer::~BoundedBuffer() {} // class destructor

void BoundedBuffer::AddToBuffer(Response response)
{
    empty.P();
    lock.P(); // critical section
    bufferQ.push(response);
    lock.V();
    full.V();
    return;
}

Response BoundedBuffer::RemoveFromBuffer()
{
    full.P();
    lock.P();
    Response poppedResponse = bufferQ.front(); // gets element from buffer
    bufferQ.pop(); // takes element off of buffer
    lock.V();
    empty.V();

    return poppedResponse;
}
