//
// Created by Jonathan G. Westerfield on 10/10/17.
//

#include "semaphore.h"

Semaphore::Semaphore(int _val)
{
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&c, NULL);
    value = _val;
}

Semaphore::~Semaphore()
{
    printf("Destroying mutex within Semaphore\n");
    pthread_mutex_destroy(&m); // destroys the mutex within the class
    printf("Destroying Semaphores condition variable\n");
    pthread_cond_destroy(&c);
    printf("Semaphore Destroyed\n\n");
}

int Semaphore::P()
{
    int errno; //errno to check standard library funtion errors
    if (errno = pthread_mutex_lock(&m) != 0)
        return errno;
    while (value <= 0) //nasty bug missed the equal, reminder to test early
        if (errno = pthread_cond_wait(&c, &m) != 0)
            return errno;
    value--;
    if (errno = pthread_mutex_unlock(&m) != 0)
        return errno;
    return 0;
}

int Semaphore::V()
{
    int errno; //errno to check standard library funtion errors
    if (errno = pthread_mutex_lock(&m) != 0)
        return errno;
    value++;
    if (errno = pthread_cond_broadcast(&c) != 0)
        return errno;
    if (errno = pthread_mutex_unlock(&m) != 0)
        return errno;
    return 0;
}