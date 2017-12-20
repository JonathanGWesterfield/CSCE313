//
// Created by Jonathan G. Westerfield on 10/10/17.
//

#include "mutex_guard.h"
#include <stdio.h>

MutexGuard::MutexGuard(Mutex & m)
{
    printf("Now guarding this mutex\n");
    this->m = &m;
    printf("Testing to see if the lock is available\n");
    if(pthread_mutex_trylock(&m.a_lock) == 0) // if it can acquire the lock
    {
        printf("Lock is available\nLocking\n\n");
        m.Lock();
    }
    else
    {
        printf("Can't acquire lock\n\n");
    }
}

MutexGuard::~MutexGuard()
{
    printf("Unlocking mutex to destroy MutexGuard\n\n");
    m->Unlock();
}
