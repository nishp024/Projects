/* list.c
 * Fall 2022
 *
 * Propose: Contains several functions which are used in wifi_support.c
 */

#include <stdlib.h>
#include <assert.h>

#include "datatypes.h"   /* defines data_t */
#include "list.h"        /* defines public functions for list ADT */

#include <stdio.h>

/* definitions for private constants used in list.c only */
#define SORTED_LIST   -123465
#define UNSORTED_LIST -632154

/* prototypes for private functions used in list.c only */
void list_debug_validate(list_t *L);

/* ----- below are the functions  ----- */

/* Allocates a new, empty list 
 *
 * If the comparison function is NULL, then the list is unsorted.
 *
 * Otherwise, the list is initially assumed to be sorted.  Note that if 
 * list_insert is used the list is changed to unsorted.  
 *
 * The field sorted can only take values SORTED_LIST or UNSORTED_LIST
 *
 * Use linked_destruct to remove and deallocate all elements on a list 
 * and the header block.
 */
list_t * list_construct(int (*compare_function)(const data_t *, const data_t *))
{
    list_t *L;

    L = (list_t *) malloc(sizeof(list_t));
    L->head = NULL;
    L->tail = NULL;
    L->current_list_size = 0;
    L->comp_proc = compare_function;
    if (compare_function == NULL)
        L->list_sorted_state = UNSORTED_LIST;
    else
        L->list_sorted_state = SORTED_LIST;

    /* the last line of this function must call validate */
    list_debug_validate(L);
    return L;
}

/* Deallocates the contents of the specified list, releasing associated memory
 * resources for other purposes.
 *
 * Free all elements in the list and the header block.
 */
void list_destruct(list_t *list_ptr)
{
    assert(list_ptr != NULL);

    /* the first line must validate the list */
    list_debug_validate(list_ptr);
    list_node_t *index;
    index = list_ptr->tail;
    while(list_ptr->tail != NULL){ //while loop which frees every node and it's data_ptr
        list_ptr->tail = list_ptr->tail->prev;
        free(index->data_ptr); //free statement
        index->data_ptr = NULL;
        free(index); //free statement
        index = list_ptr->tail;
    }
    free(list_ptr); //frees the actual header block

}

/* Return an Iterator that points to the first element in the list.  If the
 * list is empty the value that is returned in NULL.
 */
list_node_t * list_iter_front(list_t *list_ptr)
{
    assert(list_ptr != NULL);
    list_debug_validate(list_ptr);
    return list_ptr->head;

}

/* Return an Iterator that points to the last list_node_t. If the list is empty
 * then the pointer that is returned is NULL.
 */
list_node_t * list_iter_back(list_t *list_ptr)
{
    assert(list_ptr != NULL);
    list_debug_validate(list_ptr);
    return list_ptr->tail;
}

/* Advance the Iterator to the next item in the list.  
 * If the iterator points to the last item in the list, then 
 * this function returns NULL to indicate there is no next item.
 * 
 * It is a catastrophic error to call this function if the
 * iterator, idx_ptr, is null.
 */
list_node_t * list_iter_next(list_node_t * idx_ptr)
{
    assert(idx_ptr != NULL);
    return idx_ptr->next;
}

/* Obtains a pointer to an element stored in the specified list, at the
 * specified Iterator position
 * 
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from list_construct.
 *
 * idx_ptr: pointer to position of the element to be accessed.  This pointer
 *          must be obtained from list_elem_find, list_iter_front, 
 *          list_iter_back, or list_iter_next.  
 *
 * return value: pointer to the data_t element found in the list at the 
 * iterator position. A value NULL is returned if 
 *     1.  the list is empty 
 *     2.  the idx_ptr is NULL.
 */
data_t * list_access(list_t *list_ptr, list_node_t * idx_ptr)
{
    assert(list_ptr != NULL);
    if (idx_ptr == NULL || list_ptr->head == NULL)
	return NULL;

    list_node_t *temp = list_ptr->head;
    while(temp != NULL){ //iterates through the list
        if(temp->next == idx_ptr->next){ //&& temp->next != NULL){
            return temp->data_ptr; //returns the data_ptr if the iteration matches with given parameter;
        }
        temp = temp->next; //goes through temp
    }
    /* debugging function to verify that the structure of the list is valid */
    list_debug_validate(list_ptr);
    return NULL;
}

/* Finds an element in a list and returns a pointer to it.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: element against which other elements in the list are compared.
 *
 * compare_function: function to test if elem_ptr is a match to an item in the list.
 *        Returns 0 if they match and any value not equal to 0 if they do not match.
 *
 * The function returns an Iterator pointer to the list_node_t that contains 
 * the first matching element if a match if found.  If a match is not found 
 * the return value is NULL.
 *
 * Note: to get a pointer to the matching data_t memory block pass the return
 *       value from this function to the list_access function.
 */
list_node_t * list_elem_find(list_t *list_ptr, data_t *elem_ptr,
        int (*compare_function)(const data_t *, const data_t *))
{

    list_node_t *temp = list_ptr->head;
    while(temp != NULL){ //iterates through temp
        int match = compare_function(elem_ptr, temp->data_ptr); //binds compare function to variable
        if(match == 0){
            return temp; //returns the matching value if found
        }
        temp = temp->next; //temp become the value after it
    }

    list_debug_validate(list_ptr);
    /* fix the return value */
    return NULL;
}

/* Inserts the data element into the list in front of the iterator 
 * position.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the memory block to be inserted into list.
 *
 * idx_ptr: pointer to a list_node_t.  The element is to be inserted as a 
 *          member in the list at the position that is immediately in front 
 *          of the position of the provided Iterator pointer.
 *
 * If the idx_ptr pointer is NULL, the the new memory block is
 *          inserted after the last element in the list.  That is, a null
 *          iterator pointer means make the element the new tail.
 *
 * If idx_ptr is set using 
 *    -- list_iter_front, then the new element becomes the first item in 
 *       the list.
 *    -- list_iter_back, then the new element is inserted before the last item 
 *       in the list.
 *    -- for any other idx_ptr, the new element is insert before the 
 *       Iterator
 *
 * For example, to insert at the tail of the list do
 *      list_insert(mylist, myelem, NULL)
 * to insert at the front of the list do
 *      list_insert(mylist, myelem, list_iter_front(mylist))
 *
 * Note that use of this function results in the list to be marked as unsorted,
 * even if the element has been inserted in the correct position.  That is, on
 * completion of this subroutine the list_ptr->list_sorted_state must be equal 
 * to UNSORTED_LIST.
 */
void list_insert(list_t *list_ptr, data_t *elem_ptr, list_node_t * idx_ptr)
{
    assert(list_ptr != NULL);

    /* insert your code here */
    list_ptr->list_sorted_state = UNSORTED_LIST; //makes list unsorted if function is used
    list_node_t *new = (list_node_t*) malloc(sizeof(list_node_t));
    new->next = NULL;
    new->prev = NULL;
    if(idx_ptr == NULL){
        list_node_t *last = list_ptr->head; //sets last equal to the first element in the list

        new->data_ptr = elem_ptr; //fills data in new
        new->next = NULL;

        if(list_ptr->head == NULL){ //runs if list is empty
            new->prev = NULL;
            list_ptr->head = new; //head and tail is equal to only value in list
            list_ptr->tail = new;
        }
        else{
            while(last->next != NULL){
               last = last->next; //goes through the list
            }
            last->next = new;
            new->prev = last;
            list_ptr->tail = new; //new becomes the tail of the list
        }
        list_ptr->current_list_size++;
    }
    else if(idx_ptr == list_iter_front(list_ptr)){ //puts new into the front of the list
        new->data_ptr = elem_ptr;
        new->next = list_ptr->head; //puts new before the head
        new->prev = NULL;
        if(list_ptr->head != NULL){
            list_ptr->head->prev = new;
            list_ptr->head = new; //new becomes the head of the list
        }
        list_ptr->current_list_size++; 
    }
    else if (idx_ptr == list_iter_back(list_ptr)){ //puts new into the back of the list
        new->data_ptr = elem_ptr; //fills new with value
        new->prev = list_ptr->tail->prev;
        new->next = list_ptr->tail;
        list_ptr->tail->prev = new;
        if(new->prev != NULL){
            new->prev->next = new; //new become equal to the next of tail
        }
        else{
            list_ptr->head = new; //makes head equal to new
        }
        list_ptr->current_list_size++;
    }
    else{
        new->data_ptr = elem_ptr; //fills new with data

        new->prev = idx_ptr->prev; //sets prev of new equal to the prev of idx
        new->next = idx_ptr; //sets new to go before idx_ptr in the list
        idx_ptr->prev = new;

        if(new->prev != NULL){
            new->prev->next = new;
        }
        else{
            list_ptr->head = new;
        }
        list_ptr->current_list_size++;
    }
    list_ptr->tail->next = NULL; //makes sure that only the list values are not equal to NULL
    list_ptr->head->prev = NULL;

    /* insert your code here */

    /* the last two lines of this function must be the following */
    if (list_ptr->list_sorted_state == SORTED_LIST) 
	list_ptr->list_sorted_state = UNSORTED_LIST;
    list_debug_validate(list_ptr);
}

/* Inserts the element into the specified sorted list at the proper position,
 * as defined by the compare_proc.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * If you use list_insert_sorted, the list preserves its sorted nature.
 *
 * If you use list_insert, the list will be considered to be unsorted, even
 * if the element has been inserted in the correct position.
 *
 * If the list is not sorted and you call list_insert_sorted, this subroutine
 * must generate a system error and the program should immediately stop.
 *
 * The comparison procedure must accept two arguments (A and B) which are both
 * pointers to elements of type data_t.  The comparison procedure returns an
 * integer code which indicates the precedence relationship between the two
 * elements.  The integer code takes on the following values:
 *    1: A should be closer to the front of the list than B
 *   -1: B should be closer to the front of the list than A
 *    0: A and B are equal in rank
 *
 * Note: if the element to be inserted is equal in rank to an element already
 * in the list, the newly inserted element will be placed after all the
 * elements of equal rank that are already in the list.
 */
void list_insert_sorted(list_t *list_ptr, data_t *elem_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->list_sorted_state == SORTED_LIST);

    /* insert your code here */
    list_node_t *new = (list_node_t*) malloc(sizeof(list_node_t));
    new->prev = NULL; //makes sure that the new node is completely NULL
    new->next = NULL;
    new->data_ptr = elem_ptr; //fills new with data
    list_node_t *temp;
    temp = list_ptr->tail;

    if(list_ptr->current_list_size == 0){ //case if the list if empty
        list_ptr->head = new; //head and tail point to new
        list_ptr->tail = new;
    }
    else if(list_ptr->current_list_size == 1){ //case if there will be two elements in the list
        if(list_ptr->comp_proc(temp->data_ptr, new->data_ptr) == 1 || list_ptr->comp_proc(temp->data_ptr, new->data_ptr) == 0){
            list_ptr->head = temp; //temp becomes the head
            temp->next = new;
            new->prev = temp;
            list_ptr->tail = new; //new becomes the tail
        }
        else if(list_ptr->comp_proc(temp->data_ptr, new->data_ptr) == -1 ){
            list_ptr->head = new; //new becomes the head
            new->next = temp;
            temp->prev = new;
            list_ptr->tail = temp; //temp becomes the tail
        }

    }
    else{
        list_node_t *first = list_ptr->tail; //first is set to tail
        list_node_t *store = NULL;
        int complete = 0;
        while(first != NULL && complete != 1){ //while loop to insert the new element

            if(list_ptr->comp_proc(new->data_ptr, list_ptr->head->data_ptr) == 1){
                new->next = list_ptr->head; //next becomes first in list
                list_ptr->head->prev = new;
                list_ptr->head = new; //head points to next
                new->prev = NULL;
                complete = 1; //flag call
            }
            else if(list_ptr->comp_proc(new->data_ptr, list_ptr->tail->data_ptr) == -1 || list_ptr->comp_proc(new->data_ptr, list_ptr->tail->data_ptr) == 0){
            new->prev = list_ptr->tail; //next becomes last in list
            list_ptr->tail->next = new;
            list_ptr->tail = new; //tail points to next
            new->next = NULL;
            complete = 1; //flag call
            }
            else{
                if(list_ptr->comp_proc(first->data_ptr, new->data_ptr) == -1){
                    first = first->prev; //iterates through first
                }
                else{
                    store = first->next; //swaps first and new variables
                    new->prev = first;
                    new->next = store;
                    store->prev = new;
                    first->next = new;
                    complete = 1;
                }
            }
        }
    }

    /* insert your code here */
    /* the last line of this function must be the following */
    list_ptr->current_list_size++;
    list_debug_validate(list_ptr);
}

/* Removes the element from the specified list that is found at the 
 * iterator pointer.  A pointer to the data element is returned.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * idx_ptr: pointer to position of the element to be accessed.  This pointer
 *          must be obtained from list_elem_find, list_iter_front, or
 *          list_iter_next, or list_iter_back.  
 *
 *          If the idx_ptr is null, then assume that the first item
 *          at the head is to be removed.
 *
 * If the list is empty, then the function should return NULL.  Note: if
 *    the list is empty, then the iterator should be a NULL pointer.
 *
 * Note to remove the element at the front of the list use either
 *     list_remove(mylist, list_iter_front(mylist))
 *  or
 *     list_remove(mylist, NULL);
 *
 * Note: a significant danger with this function is that once
 * an element is removed from the list, the idx_ptr is dangling.  That
 * is, the idx_ptr now points to memory that is no longer valid.
 * You should not call list_iter_next on an iterator after there
 * has been a call to list_remove with the same iterator.
 *
 * When you remove the list_node_t in this function, you should null the next
 * and prev pointers before you free the memory block to avoid accidental use
 * of these now invalid pointers. 
 */
data_t * list_remove(list_t *list_ptr, list_node_t * idx_ptr)
{
    assert(list_ptr != NULL);
    if (list_ptr->current_list_size == 0)
	assert(idx_ptr == NULL);

    /* insert your code here */

    list_node_t *ptr;
    data_t * ptrd; 
    if(list_ptr->current_list_size == 0){
        return NULL;
    }
    else if(list_ptr->current_list_size == 1){
        ptr = list_ptr->head;
        ptrd = list_ptr->head->data_ptr;

        list_ptr->head = NULL;
        list_ptr->tail = NULL;
        free(ptr);
        list_ptr->current_list_size--;
        return(ptrd);
    }else if(idx_ptr == NULL || idx_ptr == list_ptr->head){
        ptr = list_ptr->head;
        list_ptr->head = list_ptr->head->next;
        list_ptr->head->prev = NULL;
        ptr->next = NULL;
        ptr->prev = NULL;
        ptrd = ptr->data_ptr;

        free(ptr);
        list_ptr->current_list_size--;
        return ptrd;
    }   
    else if(idx_ptr == list_ptr->tail){
        ptr = list_ptr->tail;
        list_ptr->tail = list_ptr->tail->prev;
        list_ptr->tail->next = NULL;
        ptr->next = NULL;
        ptr->prev = NULL;
        ptrd = ptr->data_ptr;

        free(ptr);
        list_ptr->current_list_size--;
        return ptrd;
    }
    else{
        ptr = idx_ptr;
        ptrd = idx_ptr->data_ptr;

        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
        ptr->prev = NULL;
        ptr->next = NULL;
        free(ptr);
        list_ptr->current_list_size--;
        return ptrd;

    }

    /* insert your code here */
    /* the last line should verify the list is valid after the remove */
    list_debug_validate(list_ptr);
    return NULL;  // you must fix the return value FIXED
}

/* Obtains the length of the specified list, that is, the number of elements
 * that the list contains.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * Returns an integer equal to the number of elements stored in the list.  An
 * empty list has a size of zero.
 */
int list_size(list_t *list_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->current_list_size >= 0);
    return list_ptr->current_list_size; //returns the current size of list
}


/* This function verifies that the pointers for the two-way linked list are
 * valid, and that the list size matches the number of items in the list.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 * The function produces no output if the two-way linked list is correct.  It
 * causes the program to terminate and print a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive, so an error may still exist in the
 * list even if these checks pass.
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY THAT
 * YOUR LIST IS CONSISTENT.
 */
void list_debug_validate(list_t *L)
{
    list_node_t *N;
    int count = 0;
    assert(L != NULL); 
    if (L->head == NULL) 
	assert(L->tail == NULL && L->current_list_size == 0);
    else
	assert(L->head->prev == NULL);
    if (L->tail == NULL) 
	assert(L->head == NULL && L->current_list_size == 0);
    else
	assert(L->tail->next == NULL);
    if (L->current_list_size == 0) assert(L->head == NULL && L->tail == NULL);
    if (L->current_list_size == 1) {
        assert(L->head == L->tail && L->head != NULL);
        assert(L->head->next == NULL && L->head->prev == NULL);
        assert(L->head->data_ptr != NULL);
    }
    if (L->head == L->tail && L->head != NULL) assert(L->current_list_size == 1);
    assert(L->list_sorted_state == SORTED_LIST || L->list_sorted_state == UNSORTED_LIST);
    if (L->current_list_size > 1) {
        assert(L->head != L->tail && L->head != NULL && L->tail != NULL);
        N = L->head;
        while (N != NULL) {
            assert(N->data_ptr != NULL);
            if (N->next != NULL) assert(N->next->prev == N);
            else assert(N == L->tail);
            count++;
            N = N->next;
        }
        assert(count == L->current_list_size);
    }
    if (L->list_sorted_state == SORTED_LIST && L->head != NULL) {
        N = L->head;
        while (N->next != NULL) {
            assert(L->comp_proc(N->data_ptr, N->next->data_ptr) != -1);
            N = N->next;
        }
    }
}
/* commands for vim. ts: tabstop, sts: softtabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */

