

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "my_allocator.h"
#include "free_list.h"
#define HEADERSIZE sizeof(FL_HEADER)

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*



/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/

/** I went with a singly linked list for this milestone because it was much much easier
 * for me to conceptualize and implement
 * @param _basic_block_size
 * @param _length
 * @return
 */



void block_join(FL_HEADER* block)
{
    uintptr_t current = (uintptr_t)block; // cast pointer to int for bit operations
    current = current - mem_begin;
    int index = 0;
    index = log2(block->totalLength);
    current ^= 1 << index; // toggle the log base 2 of size bit from the right
    current = current + mem_begin;
    FL_HEADER* buddy = (FL_HEADER*)current; //cast back from int to FL_HEADER pointer for operations

    if (check_header(buddy) == 1 && buddy->totalLength == block->totalLength)
    {
        if((int)(buddy - block) < 0)
        {
            block = buddy;
        }

        // remove the header to merge the block back into its buddy
        remove_header((list_length - 1 - (log2(block->totalLength) - log2(base))) * 2);
        block->totalLength = (block->totalLength * 2);
        block_join(block);
    }
}

// splits the blocks into smaller blocks
FL_HEADER* split_block(int index)
{
    // block size that you want to end with - cannot break down smaller than smallest size
    unsigned int break_size = base * pow(2,(list_length - 1 - (index / 2)));
    printf("breaking block size to index i = %d\n", index);
    FL_HEADER* current;
    index -= 2;//increase index to next size(decrease due to array holding largest block size first)

    if (index <= list_length * 2 && index >= 0)
    {
        if (FL_Array[index]==NULL) // if no blocks break next size
        {
            if (split_block(index)==NULL)
            {
                return NULL;
            }
        }

        current = FL_Array[index];

        remove_header(index);
        index += 2;
        current->totalLength = break_size; // change old header block size
        current[break_size / HEADERSIZE] = current[0]; // copy new header to right half
        current->next = &current[break_size / HEADERSIZE];
        FL_Array[index] = current;
        current = current->next;
        current->next = NULL;
        FL_Array[index + 1] = current;
        check_header(current);
        return current;
    }
    else
    {
        return NULL; // return if block couldn't be broken
    }
}

// removes header from block so that it can be rejoined
void remove_header(int index)
{
    if (FL_Array[index] != FL_Array[index + 1])//remove from list
        FL_Array[index] = FL_Array[index]->next;
    else
    {
        FL_Array[index] = NULL;
        FL_Array[index + 1] = NULL;
    }
}

// checks to see if the specified block has been allocated or not
int check_header(FL_HEADER* block)
{

    if (block->allocated == '_')  //free
    {
        printf("Freeing memory of size: %d\n",block->totalLength);
        return 1;
    }
    if (block->allocated=='!')  //used
    {
        printf("Used up memory of size: %d\n", block->totalLength);
        return 0;
    }
    return -1; // returns -1 as an ERROR code
}


unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length)
{
    unsigned int remainder = _length % _basic_block_size;
    unsigned int total = _length - remainder; // take away memory that will not fit into multiple of basic block size
    if (remainder != 0)
    {
        total = total + _basic_block_size; // increment to allocate an extra block for the left over bytes
    }

    _length = total;
    list_length = log2(total) - log2(_basic_block_size); // determine number of lists
    list_length = round(list_length + .5); // add .5 to always round up
    base = _basic_block_size; // for simplified use in other function
    FL_HEADER** free_list = (FL_HEADER**)malloc(2 * list_length * sizeof(FL_HEADER*)); // free list array

    free_list[0] = (FL_HEADER*)malloc(total); //tell os to get continuous chunk of memory and store on free_list
    mem_begin = (uintptr_t)free_list[0];

    for (int i = 1; i < list_length * 2; i++)
    {
        free_list[i] = NULL; //initalize pointers to Null
    }

    FL_HEADER* ptr = free_list[0]; //set pointer that will be used to create headers to beginning of list
    FL_HEADER head1;
    head1.allocated = '_';
    head1.next = NULL;

    printf("Size of FL_Array: %d\n\n", (int)list_length);

    for (int i = 0; i < list_length * 2; i += 2)
    {
        head1.totalLength = (_basic_block_size * pow(2, (list_length - ((i + 2) / 2))));
        // printf("\n%d Iteration %d power is: %d\n", total, i / 2, pow(2, (list_length - (((i + 2) / 2)))));

        // so that the first pointer only gets initialized once
        if (total >= (_basic_block_size * pow(2, (list_length - (((i + 2) / 2))))))
        {
            free_list[i] = ptr; //else ptr is equal to null
        }

        while (total >= (_basic_block_size * pow(2, (list_length - (((i + 2) / 2))))))
        {
            ptr[0] = head1;
            ptr->next = &ptr[(_basic_block_size * (int)(pow(2, (list_length - (((i + 2) / 2)))))) / HEADERSIZE];
            ptr = ptr->next;
            total = total - _basic_block_size * pow(2, (list_length - (((i + 2) / 2))));
            printf("Total unallocated Memory: %d\n", total);
            if (ptr->totalLength == free_list[i]->totalLength)
            {
                free_list[i + 1] = ptr;
            }
            else
            {
                free_list[i + 1] = free_list[i];
            }
        }
    }

    FL_Array = free_list;

    return  _length; //if successful return amount of memory made available
}

/**
 * My implementation of malloc
 * @param _length
 * @return my_Addr - the address the user is given to manipulate the memory blocks
 */
Addr my_malloc(size_t _length)
{
    // determine block index/size needed
    // remember all blocks have a header
    printf("Attempting to allocate %d bytes\n",_length);

    int index = 0;
    while ((_length + HEADERSIZE) / (base * pow(2, index)) > 1)
    {
        index++;
    }

    if(index >= list_length)
    {
        printf("\nERROR: my_malloc\nMemory request exceeds biggest memory block\n\n");
        return NULL;
    }

    index = list_length - 1 - index; // set index to reverse order
    if (FL_Array[index * 2] == NULL)
    {
        printf("Breaking blocks to FL_Array index i = %d\n", index * 2);
        if (split_block(index * 2) == NULL) // break blocks if it is unsuccessful
        {
            printf("\nERROR!!!\nBlock break unsuccessful\nNo memory left\nreturning NULL\n\n");
            return (void*)NULL;
        }
    }

    int headerCheck = check_header(FL_Array[index * 2]);
    if (headerCheck == 0)
    {
        printf("\nERROR my_malloc!!!\nAcessing used memory\n\n");
    }

    // if there was an \nERROR in the check_header function, let the user know
    if (headerCheck == -1)
    {
        printf("\nERROR in my malloc!!! invalid header allocated\n\n");
    }

    FL_Array[index * 2]->allocated = '!'; // set block to allocated
    Addr my_Addr = &FL_Array[index * 2][1]; // give address to free memory but do not include header

    remove_header(index * 2);
    return my_Addr;
}

int my_free(Addr _a)
{
    FL_HEADER* my_Addr = (FL_HEADER*)_a;
    int* eraser = (int*)my_Addr; // pointer to erase memory
    my_Addr = &my_Addr[-1]; // shift pointer from used mem to header

    int sz = my_Addr->totalLength;
    printf("Freeing block! Size: %d\n",sz);

    // checks to see if block has already been freed
    int headerCheck = check_header(my_Addr);
    if (headerCheck == 1)
    {
        printf("ERROR my_free\nBlock already free!\n\n");
    }
    if (headerCheck == -1)
    {
        printf("ERROR my_free\nNot a header!\n\n");
    }

    my_Addr->allocated = '_'; // set block from allocated to free
    block_join(my_Addr);
    int i = 0;
    while((my_Addr->totalLength - HEADERSIZE) > (i * sizeof(int)))
    {
        eraser[i] &= 0;
        i++;
    }
    i = 0;

    while (my_Addr->totalLength > (base * pow(2, i))) // find index
    {
        i++;
    }
    i = list_length - 1 - i; // inverse index to access list

    if (FL_Array[i * 2] == NULL) // add to list
    {
        FL_Array[i * 2] = my_Addr;
        FL_Array[i * 2 + 1] = my_Addr;
    }
    else
    {
        my_Addr->next = FL_Array[i * 2];
        FL_Array[i * 2] = my_Addr;
    }

    printf("Memory has been freed!\n\n");
    return 0;
}
int release_allocator()
{
    /* This function returns any allocated memory to the operating system.
       After this function is called, any allocation fails.
    */
    free((void*)FL_Array);
    free((void*)mem_begin);
    printf("Released and deallocated all memory\n\n");
    return 0;
}
