//
// Created by Westerfield, Jonathan on 1/2/21.
//

// Example program
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef struct fl_header
{
    unsigned int totalLength;
    unsigned int lengthAllocBlock;
    struct fl_header* next;
    struct fl_header* prev;
    bool allocated;
    unsigned int magic; // Determines if the memory has been corrupted
} FL_HEADER;

static FL_HEADER* FL_head;
static FL_HEADER* FL_tail;


// i is just for debugging purposes since it segfaults with wrong pointer arithmetic
void blankHeaderCreate(FL_HEADER* header, unsigned int blockSize, int i) {
    header->totalLength = sizeof(FL_HEADER) + blockSize;
    header->lengthAllocBlock = blockSize;
    header->next = NULL;
    header->prev = NULL;
    header->allocated = false;
    header->magic = 0;
}

/** Gives the address of the next header/block by ensuring pointer arithmetic with
 * only char types
 * ptr math: next header/block addr = (addr of this block) + (sizeof Header) + (block size)
 * Need to make sure address we are adding is a char* or else ptr arithmetic will be wrong*/
char* nextBlockAddr(char* currentAddr, unsigned int blockSize) {
    return (char*)currentAddr + sizeof(FL_HEADER) + blockSize;
}

void createList(char* salamiBlock, int numBlocks, unsigned int blockSize) {
    char* currentAddr = salamiBlock;
    FL_HEADER* thisBlock = currentAddr;
    blankHeaderCreate(thisBlock, blockSize, 0);

    for (int i = 0; i <= numBlocks; ++i) {
        if (i == numBlocks - 1) {
            FL_tail = thisBlock;
            break;
        }

        currentAddr = nextBlockAddr(currentAddr, blockSize);
        FL_HEADER* nextBlock = currentAddr;
        blankHeaderCreate(nextBlock, blockSize, i);

        thisBlock->next = nextBlock;
        nextBlock->prev = thisBlock;
        thisBlock = nextBlock;
    }
}

void printHeaderContents(FL_HEADER* header) {
    printf("totalLength: %d\n", header->totalLength);
    printf("lengthAllocBlock: %d\n", header->lengthAllocBlock);
    printf("next: %p\n", header->next);
    printf("prev: %p\n", header->prev);
    printf("allocated: %s\n", header->allocated ? "true" : "false");
    printf("magic: %d\n", header->magic);
    printf("Next - this:\n%p - %p\n = %d\n\n",
           header->next, header, ((uintptr_t)header->next - (uintptr_t)header));
}

int traverseList() {
    FL_HEADER* ptr = FL_head;
    int count = 0;

    while(ptr != NULL) {
        printHeaderContents(ptr);
        ptr = ptr->next;
        ++count;
    }
    return count;
}

int main() {
    unsigned int _length = 5242880;
    unsigned int blockSize = 128;
    int numBlocks = _length / blockSize;

    int memForBlocks = numBlocks * blockSize;
    int memForHeaders = numBlocks * sizeof(FL_HEADER);
    int totalMemoryToMalloc = memForBlocks + memForHeaders;

    char* start = (char*)malloc(totalMemoryToMalloc);
    printf("Total Memory Allocated for entire salami: %d\n", totalMemoryToMalloc);

    createList(start, numBlocks, blockSize);
    FL_head = start;

    printf("Final length of list: %d\n", traverseList());
    printf("Head: %p\n", FL_head);
    printf("Tail: %p\n", FL_tail);
    printf("Size of block + FL_HEADER:\n%d\n%d\n = %d\n", blockSize, sizeof(FL_HEADER), (sizeof(FL_HEADER) + blockSize));

    return 0;
}
