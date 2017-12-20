/* 
    File: my_allocator.c

    Author: <your name>
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#define HEADERSIZE sizeof(FL_HEADER)
#include<stdlib.h>
#include <stdio.h>
#include "my_allocator.h"
#include "free_list.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

/* Don't forget to implement "init_allocator" and "release_allocator"! */

static FL_HEADER* start;
static FL_HEADER* end;
static FL_HEADER* free_list;
static unsigned int remaining;

Addr my_malloc(size_t _length)
{
    if((_length + HEADERSIZE) > remaining)
    {
        return NULL;
    }

    FL_HEADER *current = (FL_HEADER*)start; // type cast the start header to a FL_HEADER type

    while(current->totalLength < _length)
    {
        if(current->next == NULL)
        {
            printf("Not Enough Memory Found");
            return NULL;
        }
        else
            current = current->next;
    }

    current = (FL_HEADER*)start;
    char* baseAddress = (char*)current;
    if(current->totalLength == _length + HEADERSIZE) // if it exactly equals the size
    {
        FL_remove(&free_list, (FL_HEADER*)baseAddress);
        return (Addr)baseAddress;
    }
    else // does not equal the size, now it must cut out its own size
    {
        FL_HEADER* newHeader = (FL_HEADER *)((char*)current + HEADERSIZE + _length); // base address + header + needed length
        newHeader->prev = NULL;
        newHeader->next = NULL;

        newHeader->lengthAllocBlock = _length;
        newHeader->totalLength = HEADERSIZE + _length;
        current->totalLength = _length + HEADERSIZE;

        printf("Attempting to FL_add\n");
        FL_add(&free_list, newHeader);

        printf("Attempting to Fl_remove\n");
        FL_remove(&free_list, current);
        printf("Successfully FL_removed\n");
        return (Addr)baseAddress;
    }

    printf("Something went horribly wrong in my_malloc");
    return NULL;
}

int release_allocator()
{
    free((void*)start);
}

unsigned int init_allocator(unsigned int _basic_block_size,
                            unsigned int _length)
{
    free_list = (FL_HEADER*)malloc(HEADERSIZE + _length);
    start = free_list;
    end = (FL_HEADER*)((char*)free_list + _length);
    start->totalLength = HEADERSIZE + _length;
    start->lengthAllocBlock = _length;
    remaining = _length - HEADERSIZE;
    end = (FL_HEADER*)((char*)start + _length + HEADERSIZE);

    free_list->totalLength = remaining;
    free_list->next = NULL;
    free_list->prev = NULL;

    if(start == 0)
    {
        return 0;
    }

    return _length;
}

int my_free(Addr _a)
{
    FL_HEADER* current = (FL_HEADER*)(((char*)_a) - HEADERSIZE);
    if((current >= start) && current <= end) // checking if we can actually free this memory
    {
        FL_add(&free_list, current);
        printf("Freeing memory at %x\n", current);
        return 0;
    }
    // must return the size of the memory block that it returns to the free list
    /* Same here! */
    // free(_a);
    printf("The memory you tried to free was not memory you owned\n");
    return 1;
}

