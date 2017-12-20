//
// Created by Jonathan G. Westerfield on 9/12/17.
//

#include "free_list.h"

void FL_remove(FL_HEADER **free_list, FL_HEADER * block)
{
    if (block->prev == NULL && block->next == NULL)
    {
        printf("Block is only object in list\n");
        return;
    }
    else if(block->prev == NULL) // if we are removing the head of the list
    {
        printf("Attempting to remove from beginning of FL list\n");
        *free_list = block->next;
        printf("Freelist = block->next\n");
        (*free_list)->prev = NULL;
        printf("Free list->prev = NULL\n");
        block->prev = NULL;
        block->next = NULL;
        printf("Successfully removed from beginning of FL list\n");

        return;
    }
    else if(block->next == NULL) // is removing from the very end of the list
    {
        printf("Attempting to remove from end of FL list\n");
        block->prev->next = NULL;
        block->prev = NULL;

        printf("Successfully removed from the end of FL list\n");
        return;
    }
    else // is removing from somewhere in the middle of the free list
    {
        printf("Attempting to remove from somewhere in the middle of FL list\n");

        FL_HEADER* temp = block->next;
        printf("FL_HEADER* temp = block->next;\n");
        temp->prev = block->prev;
        printf("temp->prev = block->prev;\n");
        block->prev->next = temp;
        block->prev = NULL;
        block->next = NULL;
        /*block->prev->next = block->next;
        printf("block->prev->next = block->next\n");
        block->next->prev = block->prev;
        printf("block->next->prev = block->prev;\n");
        block->prev = NULL;
        block->next = NULL;*/

        printf("Successfully removed from somewhere in the middle of FL list\n");
        return;
    }
}

void FL_add(FL_HEADER **free_list, FL_HEADER * block)
{
    block->next = *free_list;
    block->prev = NULL;
    if(*free_list != NULL)
    {
        (*free_list)->prev = block;
    }
    (*free_list) = block;

    /**free_list->prev = block;
    block->next = free_list;
    block->prev = NULL;
    free_list = block;
    // free_list->totalLength = block->totalLength;
    *free_list->totalLength += block->totalLength; // adds the block size to the free list size*/
}




