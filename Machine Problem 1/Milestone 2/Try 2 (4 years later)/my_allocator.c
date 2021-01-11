//
// Created by Westerfield, Jonathan on 12/31/20.
//
#include <stdlib.h>
#include "my_allocator.h"
#include "free_list.h"

static unsigned int memRemaining;
static unsigned minBlockSize;

// Our memory free list
static FL_HEADER* FL_head;

/** This function initializes the memory allocator and makes a portion of
 * ’_length’ bytes available. The allocator uses a ’_basic_block_size’ as
 * its minimal unit of allocation. The function returns the amount of
 * memory made available to the allocator. If an error occurred, it returns 0. */
unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length) {
    // Check to make sure invalid lengths & block sizes aren't given
    if (_length < 1 || _basic_block_size < 1)
        return 0;

    minBlockSize = _basic_block_size;
    memRemaining = _length;

    // Create the giant block of memory first to salami it later
    FL_head = (FL_HEADER*)malloc(_length);

    if (FL_head == NULL) { // make sure allocation actually succeeded
        printf("Memory allocation FAILED during FreeList creation");
        return 0;
    }

    // create new header at beginning of salami block
    blankHeaderCreate(FL_head, _length - HEADER_SZ);
    return _length;
}

/** Allocate _length number of bytes of free memory and returns the address
 * of the allocated portion. Returns 0 when out of memory. */
Addr my_malloc(unsigned int _length) {
    // 2*headersize since salami creates new header
    if (memRemaining < (_length + (2 * HEADER_SZ))) {
        printf("\nMemory Requested:\t %d\n", _length);
        printf("Memory Needed:\t\t %d\n", (_length + (2 * HEADER_SZ)));
        printf("Memory Remaining:\t %d\n", memRemaining);
        return 0;
    }

    // iterate through free list until we find a block of adequate size
    FL_HEADER* thisBlk = FL_head;
    while(thisBlk != NULL) {
        if (thisBlk->totalLength >= (_length + HEADER_SZ) && !thisBlk->allocated)
            break;
        thisBlk = thisBlk->next;
    }

    // Couldn't find a block meaning we ran out of memory
    if (thisBlk == NULL)
        return 0;

    // if we somehow found a block of exact size to what user wanted
    if (thisBlk->totalLength == _length + HEADER_SZ) {
        memRemaining -= thisBlk->totalLength;
        return FL_remove(thisBlk);
    }

    salami(thisBlk, _length, memRemaining);
    FL_remove(thisBlk);

    memRemaining -= thisBlk->totalLength;

    return (Addr)getAllocBlkAddr(thisBlk);
}

/** Frees the section of physical memory previously allocated using
 * ’my_malloc’. Returns 0 if everything ok. */
int my_free(Addr _a) {
    FL_HEADER* header = getHeaderAddr(_a);
    memRemaining += header->totalLength;

    FL_add(header);
    defrag(FL_head);

    return 0;
}
