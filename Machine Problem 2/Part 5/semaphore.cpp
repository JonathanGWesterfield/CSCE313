//
// Created by Jonathan G. Westerfield on 10/10/17.
//

#include "semaphore.h"
#include "mutex.h"

Semaphore::Semaphore(int _val)
{
    printf("Initializeing Semaphore\n");
    this->m = Mutex();
    printf("Initializing Mutex within Semaphore\n");
    pthread_mutex_init(&m.a_lock, NULL); // initialize m's lock
    printf("Initializing Condition");
    pthread_cond_init(&c, NULL);
    value = _val;
}

Semaphore::~Semaphore()
{
    printf("Destroying mutex within Semaphore\n");
    pthread_mutex_destroy(&m.a_lock); // destroys the mutex within the class
    printf("Destroying Semaphores condition variable\n");
    pthread_cond_destroy(&c);
    printf("Semaphore Destroyed\n\n");
}

int Semaphore::P()
{
    printf("P()\nLocking mutex within semaphore.P()\n");
    m.Lock();
    printf("Decrementing Value\n");
    value--;
    printf("Checking if lock is available\n");
    if(value < 0)
    {
        printf("Lock is not available\nWill wait until it is available\n");
        pthread_cond_wait(&c, &m.a_lock); // wait until the semaphore is unlocked so it can unlock
    }
    printf("Unlocking mutex within Semaphore.P()\n");
    m.Unlock();
    printf("Done\n\n");
}

int Semaphore::V()
{
    printf("V()\nLocking mutex within semaphore.V()\n");
    m.Lock();
    printf("Decrementing Value\n");
    value++;
    printf("Checking if lock is available\n");
    if(value <= 0)
    {
        printf("Lock is not available\nWill wait until it is available\n");
        pthread_cond_signal(&c);
    }

    printf("Unlocking mutex within Semaphore.P()\n");
    m.Unlock();
    printf("Done\n\n");
}