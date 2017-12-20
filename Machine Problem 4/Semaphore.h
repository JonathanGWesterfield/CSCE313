//
// Created by Jonathan G. Westerfield on 10/10/17.
//

#ifndef CLIONMP2_SEMAPHORE_H
#define CLIONMP2_SEMAPHORE_H

#include <pthread.h>
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
    pthread_mutex_t m;
    pthread_cond_t c;

public:

    Semaphore(int _val);
    ~Semaphore();

    /* -- SEMAPHORE OPERATIONS */
    int P();
    int V();
};

#endif //CLIONMP2_SEMAPHORE_H
