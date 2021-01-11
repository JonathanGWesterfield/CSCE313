//
// Created by Westerfield, Jonathan on 12/31/20.
//

#ifndef MALLOC_MY_ALLOCATOR_H
#define MALLOC_MY_ALLOCATOR_H
typedef void* Addr;

/* This function initializes the memory allocator and makes a portion of
 * ’_length’ bytes available. The allocator uses a ’_basic_block_size’ as
 * its minimal unit of allocation. The function returns the amount of
 * memory made available to the allocator. If an error occurred, it returns 0. */
unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length);

/* This function returns any allocated memory to the operating system.
 * After this function is called, any allocation fails. */
int release_allocator();

/* Allocate _length number of bytes of free memory and returns the address
 * of the allocated portion. Returns 0 when out of memory. */
Addr my_malloc(unsigned int _length);

/* Frees the section of physical memory previously allocated using
 * ’my_malloc’. Returns 0 if everything ok. */
int my_free(Addr _a);

#endif //MALLOC_MY_ALLOCATOR_H
