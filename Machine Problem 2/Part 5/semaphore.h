//
// Created by Jonathan G. Westerfield on 10/10/17.
//

#ifndef CLIONMP2_SEMAPHORE_H
#define CLIONMP2_SEMAPHORE_H

#include <pthread.h>
#include "mutex_guard.h"
#include <stdio.h>
#include <cstring>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   S e m a p h o r e  */
/*--------------------------------------------------------------------------*/

class Semaphore
{
private:
    /* -- INTERNAL DATA STRUCTURES
       You may need to change them to fit your implementation. */

    int value;
    Mutex m;
    pthread_cond_t c;

public:

    /* -- CONSTRUCTOR/DESTRUCTOR */

    Semaphore(int _val);

    ~Semaphore();

    /* -- SEMAPHORE OPERATIONS */

    int P();

    int V();
};

#endif //CLIONMP2_SEMAPHORE_H
