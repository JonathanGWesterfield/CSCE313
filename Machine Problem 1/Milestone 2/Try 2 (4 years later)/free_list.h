//
// Created by Westerfield, Jonathan on 1/1/21.
//

#ifndef MALLOC_PT_2_FREE_LIST_H
#define MALLOC_PT_2_FREE_LIST_H

#define HEADER_SZ sizeof(FL_HEADER) // don't want to call sizeof every time
#include <stdbool.h>
#include <stdio.h>

typedef struct fl_header
{
    unsigned int totalLength;
    unsigned int lengthAllocBlock;
    struct fl_header* next;
    struct fl_header* prev;
    bool allocated;
    unsigned int magic; // Determines if the memory has been corrupted
//    unsigned int XOR; // used as a way to link blocks in an XOR list. Maybe I'll use it?
} FL_HEADER;

/**
 * Doesn't actually remove the block from the free list, it just sets the
 * allocated boolean in the header to true. This behaves similar to removing
 * a block from the list but makes the implementation much simpler. USED FOR MY_MALLOC()
 * @param block Block we want to remove from the free list to return to the user.
 */
void* FL_remove(FL_HEADER* block);

/**
 * Add a block to the free list - USED FOR MY_FREE(). Doesn't actually add a block to the
 * list. Salami does that. This just marks the block as "unallocated". Effectively the same
 * but simpler to implement.
 * @param block Block we want to "add" back into the free list.
 */
void FL_add(FL_HEADER* block);

/**
 * Slice off block from larger block and add it to the freelist - S A L A M I
 * @param block Address of the large block so we can slice off a smaller block from it.
 * @param length Length of the user allocated block we want to slice off.
 * @param memRemaining The total memory remaining from the memory malloc'ed for the free list.
 */
void salami(FL_HEADER* block, unsigned int length, unsigned int memRemaining);

/**
 * Instantiates default values for each new header
 * @param header Pointer to the newly created header
 * @param blockSize Size of the block after the header
 */
void blankHeaderCreate(FL_HEADER* header, unsigned int blockSize);

/**
 * Defragments and coalesces adjacent unallocated blocks so we can reclaim memory
 * when the user frees it. Keeps us from running out of memory due to
 * fragmented list. Fewer elements in free list, the better.
 * @param head Head of the list we want to traverse and defragment.
 */
void defrag(FL_HEADER* head);

/**
 * Merges 2 blocks together. Merges 2 blocks by modifying header 1 and setting
 * second block header to NULL.
 * @param blk1 The base block. Second block will merge into this one. Get the order right.
 * @param blk2 The block to be merged.
 */
void mergeBlocks(FL_HEADER* blk1, FL_HEADER* blk2);

/**
 * Prints contents of a specified header block
 * @param header The address for the block header.
 */
void printHeaderContents(FL_HEADER* header);

/**
 * Traverses and prints the entire list. Also counts number of nodes in the list
 * @param FL_head The head of the list to traverse.
 * @return The length of the list - number of nodes in the list.
 */
int printList(FL_HEADER* FL_head);

/**
 * Shows a visual representation of the free list by displaying each node address.
 * @param head The head of the list to traverse.
 * @param memRemaining Displays the amount of memory left in the list
 */
void showList(FL_HEADER* head, unsigned int memRemaining);

/**
 * Gets the address after the header. Need to return this address to the user.
 * @param headerAddress Address of the header of the block we want to give to the user.
 * @return The address of user's memory block
 */
FL_HEADER* getAllocBlkAddr(FL_HEADER* headerAddress);

/**
 * Gives us the address of the header given the address of the user allocated block.
 * Uses pointer arithmetic. Must use char pointer for correct pointer math.
 * @param allocBlkAddr The address the user was given for their memory block
 * @return The address of the header
 */
FL_HEADER* getHeaderAddr(void* allocBlkAddr);

/** Gives the address of the next header/block by ensuring pointer arithmetic with
 * only char types
 * ptr math: next header/block addr = (addr of this block) + (sizeof Header) + (block size)
 * Need to make sure address we are adding is a char* or else ptr arithmetic will be wrong */
FL_HEADER* nextBlockAddr(FL_HEADER* currentAddr, unsigned int blockSize);

#endif //MALLOC_PT_2_FREE_LIST_H
