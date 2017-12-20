//
// Created by Jonathan G. Westerfield on 10/10/17.
//

#include "mutex.h"
#include <stdio.h>
#include <stdlib.h>

Mutex::Mutex()
{
    printf("Attempting to initialize mutex\n");
    this->a_lock = PTHREAD_MUTEX_INITIALIZER;
    printf("Initialized mutex\n\n");
}

Mutex::~Mutex()
{
    printf("Attempting to destroy mutex\n");
    pthread_mutex_destroy(&a_lock);
    printf("Destroyed Mutex\n\n");
}

void Mutex::Lock()
{
    printf("Locking mutex\n");
    pthread_mutex_lock(&a_lock);
    printf("Mutex Locked\n\n");
}

void Mutex::Unlock()
{
    printf("Unlocking mutex\n");
    pthread_mutex_unlock(&a_lock);
    printf("Mutex unlocked\n\n");
}