//
// Created by Jonathan G. Westerfield on 9/12/17.
//

#ifndef MILESTONE_2_FREE_LIST_H
#define MILESTONE_2_FREE_LIST_H

#include <stddef.h>
#include <stdbool.h>
#include <curses.h>
// #include "my_allocator.h"

typedef struct fl_header
{
    unsigned int totalLength;
    struct fl_header* next;
    struct fl_header* prev;
    bool allocated;
    unsigned int magic; // Determines if the memory has been corrupted
} FL_HEADER;

FL_HEADER** FL_Array;
double list_length;//double for rounding up
double base;//must be double for correct division
uintptr_t mem_begin;//pointer to beginning of memory block for bit calculations

void block_join(FL_HEADER* block);
FL_HEADER* split_block(int index);
void remove_header(int index);
int check_header(FL_HEADER* block);



void FL_remove(FL_HEADER **free_list, FL_HEADER * block);
/** Remove the given block from given free list. The free-list pointer points
 * to the first block in the free list. Depending on the implementation, you may not
 * need the free-list pointer
 */

void FL_add(FL_HEADER **free_list, FL_HEADER * block);
/** Add a block to the free list */



#endif //MILESTONE_2_FREE_LIST_H



