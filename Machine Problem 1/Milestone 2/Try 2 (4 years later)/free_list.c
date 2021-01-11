//
// Created by Westerfield, Jonathan on 1/1/21.
//

#include "free_list.h"

/**
 * Doesn't actually remove the block from the free list, it just sets the
 * allocated boolean in the header to true. This behaves similar to removing
 * a block from the list but makes the implementation much simpler. USED FOR MY_MALLOC()
 * @param block Block we want to remove from the free list to return to the user.
 */
void* FL_remove(FL_HEADER* block) {
    block->allocated = true;
}

/**
 * Add a block to the free list - USED FOR MY_FREE(). Doesn't actually add a block to the
 * list. Salami does that. This just marks the block as "unallocated". Effectively the same
 * but simpler to implement.
 * @param block Block we want to "add" back into the free list.
 */
void FL_add(FL_HEADER* block) {
    block->allocated = false;
}

/**
 * Slice off block from larger block and add it to the freelist - S A L A M I
 * @param block Address of the large block so we can slice off a smaller block from it.
 * @param length Length of the user allocated block we want to slice off.
 * @param memRemaining The total memory remaining from the memory malloc'ed for the free list.
 */
void salami(FL_HEADER* block, unsigned int length, unsigned int memRemaining) {
    FL_HEADER* newHead = nextBlockAddr((char*)block, length);
    blankHeaderCreate(newHead, memRemaining - (length + HEADER_SZ));

    block->lengthAllocBlock = length;
    block->totalLength = HEADER_SZ + length;
    newHead->next = block->next;
    newHead->prev = block;
    if (block->next != NULL)
        block->next->prev = newHead;

    block->next = newHead;
}

/**
 * Instantiates default values for each new header
 * @param header Pointer to the newly created header
 * @param blockSize Size of the block after the header
 */
void blankHeaderCreate(FL_HEADER* header, unsigned int blockSize) {
    header->totalLength = HEADER_SZ + blockSize;
    header->lengthAllocBlock = blockSize;
    header->next = NULL;
    header->prev = NULL;
    header->allocated = false;
    header->magic = 0;
}

/**
 * Defragments and coalesces adjacent unallocated blocks so we can reclaim memory
 * when the user frees it. Keeps us from running out of memory due to
 * fragmented list. Fewer elements in free list, the better.
 * @param head Head of the list we want to traverse and defragment.
 */
void defrag(FL_HEADER* head) {
    FL_HEADER* thisBlk = head;

    // Can be done recursively as well
    while(thisBlk != NULL) {
        if (thisBlk->allocated && thisBlk->next != NULL) { // don't merge allocated blocks
            thisBlk = thisBlk->next;
            continue;
        }

        while (thisBlk->next != NULL &&  thisBlk->next->allocated == false) {
            // not necessary to check pointer math since we never actually remove from freelist
            mergeBlocks(thisBlk, thisBlk->next);
        }

        if (thisBlk->next != NULL)
            thisBlk = thisBlk->next;

        break;
    }
}

/**
 * Merges 2 blocks together. Merges 2 blocks by modifying header 1 and setting
 * second block header to NULL.
 * @param blk1 The base block. Second block will merge into this one. Get the order right.
 * @param blk2 The block to be merged.
 */
void mergeBlocks(FL_HEADER* blk1, FL_HEADER* blk2) {
    blk1->totalLength += blk2->totalLength;
    blk1->lengthAllocBlock += blk2->totalLength;
    blk1->next = blk2->next;

    if (blk2->next != NULL)
        blk2->next->prev = blk1;
    blk2 = NULL;
}

/**
 * Prints contents of a specified header block
 * @param header The address for the block header.
 */
void printHeaderContents(FL_HEADER* header) {
    printf("totalLength: %d\n", header->totalLength);
    printf("lengthAllocBlock: %d\n", header->lengthAllocBlock);
    printf("next: %p\n", header->next);
    printf("prev: %p\n", header->prev);
    printf("allocated: %s\n", header->allocated ? "true" : "false");
    printf("magic: %d\n", header->magic);
    printf("Next - this = total size of header and block\n%p - %p = %d\n\n",
           header->next, header, ((uintptr_t)header->next - (uintptr_t)header));
}

/**
 * Traverses and prints the entire list. Also counts number of nodes in the list
 * @param FL_head The head of the list to traverse.
 * @return The length of the list - number of nodes in the list.
 */
int printList(FL_HEADER* head) {
    FL_HEADER* ptr = head;
    int count = 0;

    while(ptr != NULL) {
        printHeaderContents(ptr);
        ptr = ptr->next;
        ++count;
    }
    return count;
}

/**
 * Shows a visual representation of the free list by displaying each node address.
 * @param head The head of the list to traverse.
 * @param memRemaining Displays the amount of memory left in the list
 */
void showList(FL_HEADER* head, unsigned int memRemaining) {
    FL_HEADER* ptr = head; // copy to ptr to make semantically easier
    int count = 0;
    printf("%p -> ", ptr);

    while(ptr != NULL) {
        printf("%p -> ", ptr->next);
        ptr = ptr->next;
        ++count;
    }
    printf("\nList Length: %d\nMemory Left: %d\n\n", count, memRemaining);
}

/**
 * Gets the address after the header. Need to return this address to the user.
 * @param headerAddress Address of the header of the block we want to give to the user.
 * @return The address of user's memory block
 */
FL_HEADER* getAllocBlkAddr(FL_HEADER* headerAddress) {
    return (FL_HEADER*)((char*)headerAddress + HEADER_SZ);
}

/**
 * Gives us the address of the header given the address of the user allocated block.
 * Uses pointer arithmetic. Must use char pointer for correct pointer math.
 * @param allocBlkAddr The address the user was given for their memory block
 * @return The address of the header
 */
FL_HEADER* getHeaderAddr(void* allocBlkAddr) {
    return ((char*)allocBlkAddr - HEADER_SZ);
}

/**
 * Gives use the address of the next header.
 * ptr math: next header/block addr = (addr of this block) + (sizeof Header) + (block size)
 * @param currentAddr Our starting address. Must be a header address.
 * @param blockSize The size of the block after the header - User allocated block.
 * @return The address of the next block (whether or not it exists yet).
 */
FL_HEADER* nextBlockAddr(FL_HEADER* currentAddr, unsigned int blockSize) {
   return ((char*)currentAddr + HEADER_SZ + blockSize);
}

