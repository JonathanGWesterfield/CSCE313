//
// Created by Jonathan G. Westerfield on 10/10/17.
//

#ifndef CLIONMP2_MUTEX_H
#define CLIONMP2_MUTEX_H

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   M u t e x  */
/*--------------------------------------------------------------------------*/

class Mutex {
private:
    /* -- INTERNAL DATA STRUCTURES
       You may need to change them to fit your implementation. */

public:
    /* -- CONSTRUCTOR/DESTRUCTOR */
    pthread_mutex_t a_lock;
    Mutex();

    ~Mutex();

    /* -- MUTEX OPERATIONS */

    void Lock();
    void Unlock();
};

#endif //CLIONMP2_MUTEX_H
